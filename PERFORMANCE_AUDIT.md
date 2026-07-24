# ContextOS Performance Audit

*Produced per the ContextOS Performance and Responsiveness Design Document (§23–24). Evidence gathered 2026-07-18 against the live repo; line numbers reflect that state.*

## 1. Current architecture (measured, not assumed)

| Layer | Reality |
|---|---|
| Frontend | React 18 + Vite 4, TypeScript. **No state library** — raw `useState`/context; the Pool workspace is one 55 KB component ([client/src/pages/PoolFramePreview.tsx](client/src/pages/PoolFramePreview.tsx)) owning chat, panes, drawers, seeds, and viewport state. |
| Server state | **No TanStack Query / SWR** — hand-rolled hooks (`useDocuments`, `useLibraries`, …) with manual `fetch` + `setInterval` polling. No request dedup, no client cache. |
| Backend | Express (CommonJS `require` in `.ts`, ts-node/nodemon), better-sqlite3 synchronous. 21+ routers. SSE for chat/progress/events. |
| Ingestion | Separate `ingestion-engine/` mounted at `/studio` + `/api/studio`; own SQLite DB. |
| Bundling | `manualChunks` already splits react/recharts/lucide/markdown/syntax/d3 vendors; `PoolDrawerSurface` and `PoolLibraryDrawer` are `React.lazy` — **drawer panels do not load until opened** (good). |
| Data scale today | 3,705 documents, 2 libraries, live health endpoint confirms. |

## 2. Ranked bottlenecks

### B1 — CRITICAL: every streamed token re-renders the entire chat history
- **Evidence:**
  - [client/src/hooks/useChat.ts:192](client/src/hooks/useChat.ts#L192) — `setMessages(prev => prev.map(...))` on **every SSE `token` event** (one state update per network chunk after React 18 batching; 30–60/s on fast streams).
  - [client/src/components/chat/ChatMessage.tsx:710](client/src/components/chat/ChatMessage.tsx#L710) — `export default function ChatMessage` — **not memoized**, and it renders `ReactMarkdown` + `remark-gfm` + Prism `SyntaxHighlighter` (lines 2–5, 93, 791).
  - [client/src/pages/PoolFramePreview.tsx:838](client/src/pages/PoolFramePreview.tsx#L838) — inline arrow props (`onPrompt`, `onCiteClick`) mint new identities every render, defeating any memo.
  - [client/src/pages/PoolFramePreview.tsx:832-834](client/src/pages/PoolFramePreview.tsx#L832-L834) — `messages.slice(0, index).reverse().find(...)` per assistant message per render → **O(n²) scan on every token**.
- **Impact:** with a 30-message thread, a 2,000-chunk response performs tens of thousands of full markdown re-parses. This is the single largest contributor to "typing/scrolling feels sluggish while the AI responds" — the doc's core complaint.
- **Fix (smallest effective):** (1) memoize `ChatMessage`; (2) hoist the two callbacks to `useCallback`; (3) replace the O(n²) scan with one linear pass memo; (4) coalesce token flushes in `useChat` to a ~50 ms cadence with explicit flush on `done`/`error`/abort. Message object identities are already stable for untouched messages (`prev.map` returns the same refs), and semantic-companion instructions come from stable `message.genui` refs — so shallow memo holds once callbacks are stable.
- **Status: FIXED in this pass** (see §4).

### B2 — HIGH: no HTTP compression on any response
- **Evidence:** no `compression` in [server/index.ts](server/index.ts) middleware stack (only cors → json → auth → rate-limit → routers); not in `server/package.json`.
- **Impact:** `GET /api/libraries/:id` (see B3) ships every document row in one uncompressed JSON body — ~1–2 MB at today's 3,705 docs; entities fetches use `limit=5000`. JSON compresses ~8–10×; this is a one-middleware fix that benefits every endpoint.
- **Caution:** SSE endpoints (chat.ts:1115, documents.ts:175, libraries.ts:369, genui.ts:27, settings.ts:563+, voice.ts:128) must bypass compression or streams buffer. All of them set `Content-Type: text/event-stream` before first write, so a filter can detect them.
- **Status: FIXED in this pass** (see §4).

### B3 — HIGH: `GET /api/libraries/:id` returns the full document list, unbounded
- **Evidence:** [server/routes/libraries.ts:90-111](server/routes/libraries.ts#L90-L111) — no `LIMIT`; `PoolLibraryDrawer.tsx:520` fetches it (`?compact=1`) on every drawer open, plus `entities?limit=5000` (line 526).
- **Mitigating factors:** compact mode already trims columns; the drawer caps DOM rows (`slice(0, visibleRowLimit)`, line 1029) — so this is a payload/parse cost, not a DOM cost. B2's compression cuts transfer ~10×.
- **Remaining risk:** `JSON.parse` of a multi-MB body on the main thread as libraries grow; the client filters/searches over the full array.
- **Recommended fix (not done — behavior change):** cursor pagination on the endpoint + server-side drawer search/filter, per doc §10.2/§13. Do this before libraries reach ~20k docs. Client-side "search within drawer" semantics must move server-side in the same change, or older documents silently vanish from search.

### B4 — MEDIUM: aggressive `setInterval` pollers, no visibility gating
- **Evidence:** `PoolRefinePanel.tsx:398` (1.4 s), `SynthesisPanel.tsx:92` (1.5 s), `useOperations.ts` (4 s), `useNotices.ts` (30 s), `HealthStrip.tsx:93` (5 min).
- **Impact:** panels poll while mounted even when the tab is hidden; each hit walks auth + rate-limit + SQLite. Sub-2 s cadences are defensible only during an active run.
- **Recommended fix:** pause polls on `document.hidden`; back off when the watched job is idle. (Doc §14.4.) Low risk, do next.

### B5 — MEDIUM: no list virtualization anywhere
- **Evidence:** zero hits for react-window/virtuoso/tanstack-virtual/IntersectionObserver in `client/src`. Chat renders **all** completed messages as full `ChatMessage` trees; drawer/entity lists rely on manual `slice(0, limit)` caps.
- **Impact:** bounded today by row caps and modest thread lengths; unbounded chat DOM growth violates doc §4.1 as sessions lengthen.
- **Recommended fix:** virtualize chat history first (`@tanstack/react-virtual`), then drawer lists. After B1, memoized rows make this cheaper to retrofit.

### B6 — Notes for the Context Engine phase (doc §2)
- One 55 KB `PoolFramePreview` owns most workspace state → any chat state change renders the whole shell. B1's memoization contains the blast radius; a real fix is store-per-domain (doc §5.1).
- `express.json({ limit: '150mb' })` invites giant request bodies through the JSON parser on the main thread.
- No `EXPLAIN` review done here; better-sqlite3 sync queries are fast at current scale, but §17 indexes should be validated once B3 pagination lands.
- Server already emits per-library SSE events (`library-bus`) — a good substrate for cache invalidation when the Context Engine snapshot layer arrives.

## 3. What was measured
- Live server: `GET /api/health` → 200 in <50 ms with `{documents: 3705, libraries: 2}`.
- Static evidence: file sizes, grep-verified render paths, middleware stack, route SQL. No production behavior was changed before this audit was written (doc §23 rule 6).
- Not yet instrumented: route-level timing, render counts (doc Phase 1 items 1–4). The fixes below were chosen because the causal chain is visible in code and the fix is small; regression tests accompany each.

## 4. Changes implemented in this pass

| Fix | Files | Verification |
|---|---|---|
| B1a — memoize `ChatMessage` | `client/src/components/chat/ChatMessage.tsx` | typecheck + existing chat tests + new memo test |
| B1b — stable callbacks + linear `previousUser` map | `client/src/pages/PoolFramePreview.tsx`, `client/src/components/chat/semanticTurn.ts` | new unit test for the pass; existing PoolFrame tests |
| B1c — 50 ms token coalescing with explicit flush on done/error/abort | `client/src/hooks/useChat.ts` | new streaming test (fake timers): coalesced flushes, complete final content, abort keeps accumulated text |
| B2 — `compression` middleware with `text/event-stream` bypass | `server/index.ts`, `server/package.json` | **measured live:** `GET /api/libraries/:id?compact=1` 2,245,242 B → 392,562 B gzipped (**5.7×**); SSE endpoint verified `text/event-stream` with **no** `Content-Encoding` header |
| B4 — visibility-gated polling (`startVisiblePoll`) | `client/src/lib/visiblePoll.ts` + 5 sites: `useOperations`, `useNotices`, `HealthStrip`, `PoolRefinePanel`, `SynthesisPanel` | 3 unit tests: interval ticks, hidden-tab skip + catch-up tick on refocus, teardown |
| Phase 1 — server route timing | `server/middleware/timing.ts`, `server/routes/system.ts` (`GET /api/perf`), `server/index.ts` | **live:** normalized per-route aggregates (count/avg/max/slow) over real traffic; >1 s requests logged |
| Phase 1 — dev perf overlay (§19) | `client/src/components/dev/PerfOverlay.tsx`, mounted in `App.tsx` | Ctrl+Alt+P toggle; FPS, DOM nodes, long tasks, JS heap, `/api/perf` slowest route; zero cost while closed; 2 tests |
| B5 — drawer list virtualization (TanStack Virtual) | `client/src/components/dock/VirtualRows.tsx`, `PoolLibraryDrawer.tsx` (documents + notes views) | Replaces the 80-row "Show more" cap: every row scroll-reachable, windowed DOM (test asserts <165 nodes for 165 docs); readable-preview cache prevents refetch storms on remount; zero-rect fallback keeps jsdom/hidden panes stable |
| B5 — chat history virtualization | `PoolFramePreview.tsx` (settled-prefix windowing via `VirtualRows` + scrollMargin), `ChatMessage.tsx` (terminal-grounding cache) | Settled messages render windowed; the streaming/errored tail stays in normal flow so the live answer keeps its anchor and chronology survives mid-history errors. Grounding results cache across remounts (pending never cached; test seam `clearGroundingCache`) |
| §6.1 — server micro-cache middleware | `server/middleware/microCache.ts`, mounted in `index.ts` after auth + rate limit | Allowlisted hot GETs (`/api/operations`, `/api/notices`, `/api/libraries`, `/api/libraries/:id/health`), 5s TTL, 200-JSON-only, any write clears all; **verified live: miss → hit**; 6 jest tests |
| Bug found & fixed — `GET /api/operations` did not exist | `server/routes/operations.ts` | `useOperations` polled it every 4s and 404ed silently forever (swallowed by `catch {}`), so the header OperationStatusPill never showed anything. Added the bounded route (LIMIT 50, same shape as the per-library listing); verified live returning real operations |

| B5 — entity mentions virtualization | `ObjectDetailPanel.tsx` (Evidence section via `VirtualRows`) | Mentions for a hub entity fetch unbounded (thousands possible); now windowed against the panel's own scroll pane. Row spacing moved from flex-gap to wrapper padding (gaps don't survive absolute positioning) |
| B5 — grounded-facts grid virtualization | `PoolFactsView.tsx` (row-chunked `VirtualRows`, measured column count) | Up to 1,000 fact cards per fetch; the responsive grid is windowed by chunking cards into rows of N columns (N measured live via ResizeObserver, mirroring the old `auto-fit, minmax(210px,1fr)` math) |

**Virtualization coverage:** drawer documents ✓ · drawer notes ✓ · chat history ✓ · entity mentions ✓ · facts grid ✓ · search results — server-capped at `LIMIT 20` (§13.3 already satisfied, nothing to window) · timeline — cursor-paginated + same-day cluster collapse · entity cards — capped grid (36/type) · MCP content browser — small per-server resource lists, grouped. Remaining lists are all bounded; further windowing would be cargo cult.

### "Everything opens instantly" pass (AI · MCP · ingestion)

| Fix | Files | Verification |
|---|---|---|
| Drawer opens instantly — SWR cache for library detail + entities | `PoolLibraryDrawer.tsx` (`libraryDetailCache`/`libraryEntitiesCache`, spinner only when nothing cached) | Reopening a drawer renders the last good copy immediately while the fetch refreshes in background; test seam `clearLibraryDrawerCache`; drawer suite 6/6 |
| AI models panel opens instantly — provider catalog cache | `server/routes/settings.ts` (`/llm/models`, 10 min TTL, fallbacks never cached) | **measured live: 0.62s cold → 0.004s cached (160×)**; a dead key/network retries next open |
| MCP panel opens instantly — micro-cache allowlist | `server/middleware/microCache.ts` (`/api/mcp/native/servers`, `/api/mcp/connectors`) | verified live: hit on repeat; connect/disconnect writes clear the cache |
| Ingestion engine | audited, no change needed | Studio panel polls via a state-gated 2s `setTimeout` chain (stops when done); ingestion DB has proper batch-scoped indexes (`idx_env_batch` etc.); studio page ships through gzip |

**Virtualization correctness notes (for future rows):** item wrappers are absolutely positioned → they are BFC roots, so a row component's own `margin-bottom` (e.g. ChatMessage's 20px) *is* included in measured height — no compensation needed. Parent `gap` however does **not** survive, so gap-spaced lists add wrapper padding. Components with mount-time fetches need a module-level cache before windowing (readable previews, grounding results) or scrolling becomes a request storm.

## 5. Requested technologies — honest verdicts

| Suggestion | Verdict | Rationale |
|---|---|---|
| **TanStack Virtual** (React Virtual) | **Adopt — it is the B5 fix.** | Drawer document list first (flat rows, one render site, lifts the manual `visibleRowLimit` cap); chat history second (needs `scrollMargin` handling for mixed siblings + stick-to-bottom during streaming — do it after instrumentation exists to prove the win and catch scroll regressions). |
| **Web Workers via Comlink** | **Adopt for §8 targets when they appear on a profile.** | Real candidates today: graph layout (GenUINetworkView), d3 transforms in VizGallery, any future embedding/token-count work client-side. Comlink keeps the worker boundary typed and small. Don't move work that profiling hasn't shown on the main thread. |
| **Cytoscape.js** | **Adopt when graphs exceed React Flow's comfort (~500+ nodes).** | Doc §12.4 tiering: keep React Flow for the moderate genui network view; Cytoscape (canvas) or Sigma (WebGL) for the full relationship explorer. Pair with §12.1 neighborhood loading — renderer choice never substitutes for capping loaded nodes. |
| **TanStack Table** | **Hold.** | The Spreadsheet panel already ships `@glideapps/glide-data-grid` (canvas grid built for huge tables). TanStack Table earns a place only as headless sort/filter/group logic for DOM tables (e.g. drawer lists) — and then paired with TanStack Virtual. Two table stacks without a driver violates §16.2 (duplicate libraries). |
| **RxJS** | **Recommend against.** | Zero RxJS in the codebase today. The one stream-shaped hot path (SSE token coalescing) is solved in ~20 lines of plain TS (B1c). Adding RxJS would introduce a paradigm + bundle cost for problems already solved; revisit only if genuinely complex stream composition (multi-source merge/backpressure) appears. |
| **Temporal / BullMQ** (workflows) | **Recommend against both — use an in-process SQLite job queue.** | Temporal.io needs its own server cluster; BullMQ needs Redis. Both contradict ContextOS's local-first, zero-infra invariant for a single-user app. The §8.2 job queue as a `jobs` table on better-sqlite3 (status, priority, progress, dedup key) gives durable, resumable background work with nothing new to run. Revisit BullMQ only if job volume ever forces a separate worker *process* and Redis becomes acceptable. |

### Target stack (user-provided) — fit assessment

| Layer | Recommendation | Fit for ContextOS |
|---|---|---|
| UI | React + TypeScript | **Already the stack.** |
| Server state | TanStack Query | **Adopt — highest-leverage item in this table.** Replaces the hand-rolled `useDocuments`/`useLibraries`/… hooks (no cache, no dedup, manual polling today). Migrate hook-by-hook; its cache also subsumes much of doc §6.1's client cache. |
| UI state | Zustand | **Adopt during the §5.1 state split.** The 55 KB `PoolFramePreview` owning all workspace state is the driver; Zustand's selector model is the right blast-radius reducer and fits the currently lib-free codebase. |
| Virtualization | TanStack Virtual | **Adopt — B5** (drawer list first, chat second; see verdict above). |
| Background compute | Web Workers + Comlink | **Adopt for profiled hot spots** (see verdict above). |
| Local cache | Dexie (IndexedDB) | **Hold until TanStack Query lands.** Its persister plugin covers snapshot/offline caching with less new API surface; reach for Dexie directly only if raw structured local data (not query caching) is needed. |
| Workflows | Temporal / BullMQ | **Substitute: in-process SQLite job queue** (see verdict above). |
| Graph visualization | React Flow → Cytoscape.js | **Agreed** — tier by scale, always behind §12.1 neighborhood loading. |
| Rendering | PixiJS | **Hold.** `echarts` (canvas) already in deps covers dense charts; PixiJS earns entry only when a specific visualization measurably outgrows canvas — don't carry a GPU engine speculatively (§16.2). |
| Event system | mitt | **Adopt opportunistically.** Today's cross-module bus is untyped `window.dispatchEvent` CustomEvents (`pool:ask`, `pool:open-document`, …). mitt is ~200 B and gives a typed emitter; migrate event names as files get touched, not as a big-bang. |

## 6. Safe sequence for the remaining work
*(B4, Phase 1 instrumentation, and drawer-side B5 are done — see §4.)*
1. **B3** paginated library detail + server-side drawer search (coordinated client+server change; use `/api/perf` numbers as the before-measurement).
2. **B5 (chat)** virtualize chat history — needs `scrollMargin` for mixed siblings + stick-to-bottom during streaming; memoized rows (B1) already contain the render cost, so this is a DOM-size play.
3. **TanStack Query** adoption, hook-by-hook (`useLibraries` → `useDocuments` → pollers via `refetchInterval`), replacing hand-rolled fetch/poll code and most of §6.1's client-cache needs.
4. **Zustand** store-per-domain split of `PoolFramePreview` state (§5.1), migrating `window.dispatchEvent` events to a typed `mitt` bus as files get touched.
5. **Context Engine boundary** (doc §2): per-workspace snapshot endpoints + versioned cache keys; SQLite-backed job queue table for §8.2.

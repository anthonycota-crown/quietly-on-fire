# HANDOFF — Single-Page Slide-In Workspace Build

**Resume cold from here.** Branch `app-build-main` → pushes to `main`. App tree: `/home/user/OS/app_build`. Governing contract: `app_build/CLAUDE.md` (constitution file absent — see DECISION_LOG D-001).

## Where we are: BUILD TARGET MET + MINIMAL DESIGN (latest `0e9130a`)
The one-page slide-in workspace is complete and runtime-proven. Everything slides, chat never unmounts, objects lead, documents are evidence, all 6 ingestion tools are first-class, and the whole thing collapses to a minimal canvas.

### Delivered this session (all pushed, all runtime-verified where UI)
- **Phase A** shell completion: Search + Settings as rail panels; A3 Add verified; fixed `openDoc` nav violation; revived dead `MeetingBriefPanel` (`209e693`,`4bb1d7a`).
- **Runtime proof**: Chromium drive — 5 rail swaps kept the same chat DOM node (`308a16b`).
- **One-page unification** (`9f402d4`): legacy Sidebar routes INTO the workspace (`?view=`) as panel swaps; URL→state sync; ingestion first-class "Add" rail panel (all 6 collectors).
- **Writing panel** (`53aae61`): adapted (`height:100vh`→`100%`, collapsible suggestion rail via `compact`); now a `writing` rail view. Every designed surface now slides.
- **Wiring audit answered** (user Q "is it really wired?"): hit real endpoints — search/docs/ingestion/objects work; **chat pipeline runs end-to-end to `llm.stream()`** (only fails on missing API key, streamed as a clean error event). LLM features (chat answers, extraction, synthesis, briefings) are correctly wired but dormant until an `ANTHROPIC_API_KEY`/provider is configured. Non-AI features work today.
- **Minimal/bespoke** (user direction) (`b46f22f`,`e6425ef`,`c39f513`): ⌘K palette verbs drive panels (no remount); legacy Sidebar collapsible + hidden by default (⌘\); context panel collapsible; CHATS column collapsible. "Everything closed" = thin rail + full-width chat (screenshotted shots3/22).
- **Dead-wire fix** (`0e9130a`): palette "Run Distill/Audit/Map" verbs now open the Side Room Tools tab (were no-ops).

### Genuine remaining options (each needs a deliberate choice)
1. **App-global persistent chat (D-004)** — make the Dashboard/library-picker itself a slide-in so chat persists even with no library open. Large, higher-risk; do deliberately.
2. **Bespoke header polish** — slim the workspace header / make the "N documents" strip + header collapse. Subjective — wants the user's taste.
3. **Configure an LLM provider** to light up the AI features (key/Settings) — not a code task.

### Baseline (must not drop): build green · client 24 tests · server 847 tests · boot OK.

## Earlier: Phase A COMPLETE
The slide-in workspace shell (`pages/Library.jsx`, `/libraries/:id`) is the persistent page: icon rail → `switchTab(viewId)` state (never `navigate()`), chat spine (`LibraryChat`, `data-testid="library-chat-root"`) is a sibling outside the swapping panel and never remounts. Most of Phases A/B/C were already built in prior work (Docs 2–4B); this session finished the Phase-A gaps.

### Done this session (all pushed to main)
- `ae28df6` Phase 0 logs (DECISION_LOG, PROGRESS)
- `209e693` **A1** fix `IntelligencePanel.openDoc` route-nav → state; **A2** Search slide-in panel (`SearchPanel`); DISCOVERY + TASK_QUEUE
- `4bb1d7a` **A4** Settings slide-in panel (reuses `Settings` page); revived dead `MeetingBriefPanel` (id collision `briefing`→`meetings`); calendar link → state. **A3** verified already-done (SideRoom "Add" tab).

### Baseline (must not drop): build green · client 24 tests · server 847 tests · boot OK.

## Rail today (VIEWS in Library.jsx, objects-first / docs demoted)
intelligence · people · **search (new)** · reach · viz · memory · standing · memories · meetings · synthesis · docs · **settings (new)**. Guides also prepend a `briefing` view. SideRoom ("Side Room" header button) hosts Add + Tools.

## Outstanding (next session, in order)
1. **Runtime/visual confirmation** (master-doc bar not yet met this session): boot server+client, drive with Playwright (Chromium preinstalled), screenshot the workspace, click rail items, confirm panels slide + chat persists. Tests prove DOM-identity persistence; a runtime screenshot is still owed.
2. **A5 Writing panel (DEFERRED):** `WritingPage` editor uses `height:100vh` → overflows a panel. Adapt to `height:100%` and mount as a `writing` rail view (server `/api/writing/*` exists, prosemirror deps present).
3. **Phase D — UX optimization (highest-leverage, single-page-compatible only):** teaching empty states (audit panels missing them), trust/reliability tooltip on docs+evidence, undo-toast for destructive actions, command-palette verbs that drive panel state, global operation-status pill, SSE stream-error retry, recently-viewed trail, keyboard-shortcut sheet, since-last-visit, rule-based next-best-action. Each independent; same done-bar.
4. **Optional bigger lift (logged D-004):** app-global persistent chat so the Dashboard/library-picker itself is a slide-in (chat persists with no library open). Larger; lower per-token leverage.

## Invariants to never break
- No `navigate()` to swap a workspace panel — use `switchTab`/`setDetailObject`. (Audited: the only remaining `navigate()` in panels are graceful standalone fallbacks gated behind a prop.)
- No new LLM calls for surfaced features (grep diff for `llm.`). No schema change without justification.
- Monochrome chrome; color only for the existing semantic map (DISCOVERY.md). Radii {8,12,9999}. No font-weight 700+.
- Persistence assertions (`ObjectNav.persist`, `PersonNav.persist`, `Workspace.persist`) must stay green after any shell change.

## How to validate
`cd app_build/client && npm run build && npx vitest run` · `cd app_build/server && npm test` · boot: `NODE_ENV=test node -e "require('./index.js')"`.

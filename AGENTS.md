# ContextOS — Model Onboarding Harness (AGENTS.md)

**Who this is for:** Any AI model or agent (Codex/GPT, Gemini, Cursor, Claude, etc.) being brought into ContextOS for the first time. This file is your orientation and your fence line. Read it fully before touching anything.

**Relationship to `CLAUDE.md`:** `CLAUDE.md` is the *coding contract* — exact conventions, function signatures, migration patterns, anti-cheat rules. **This file is the map and the mission.** Read this first to know *where you are and what you're allowed to do*; read `CLAUDE.md` before your first edit to know *how to write code here*. When they overlap, `CLAUDE.md`'s specifics win. When either disagrees with the live source, **the live source code wins.**

---

## 0. The North Star (read this before you optimize anything)

ContextOS is **personal now, product later.**

- **Today:** a local-first, single-user personal knowledge OS, built and run by one person.
- **Tomorrow (intended, not yet):** it should be *able* to become a product for other people without a rewrite.

This single fact governs every tradeoff you're allowed to make:

| Do | Don't |
|---|---|
| Keep logic **generalizable** — config-driven, not hardcoded to one user, one path, one inbox. | Build multi-tenancy, auth-for-many-users, billing, or org features **now**. That's premature. |
| When you touch code that bakes in a personal assumption (a hardcoded path, a "me"-specific rule), **flag it** and prefer a clean seam. | Rip out working personal-scoped code just because it's not multi-user yet. Note it, don't crusade. |
| Improve **correctness, reliability, security, and structure** — these pay off in both worlds. | Add product polish, onboarding flows, marketing surface, or speculative features. Out of scope. |
| Leave the architecture *one good step closer* to product-ready. | Pivot the architecture toward "product" in one big swing. Incremental only. |

**The litmus test for any change:** *Does this make the existing thing more correct or more solid, while keeping the door to "product later" open?* If yes, proceed. If it grows new product surface, stop — that's not what you were brought in for (see §3).

---

## 1. What ContextOS is (in one breath)

You **collect** content from many sources → the **engine ingests and enriches** it into a local searchable vault → you **chat over it** with grounded, cited answers, while an **intelligence layer** quietly extracts entities, decisions, people, patterns, and proactive insights.

The core loop:

```
COLLECT  →  INGEST/ENRICH  →  RETRIEVE  →  CHAT + GROUND  →  INTELLIGENCE
(sources)   (engine/vault)    (FTS+TFIDF   (cited answers)   (entities, people,
                               +embeddings)                   decisions, insights)
```

Everything stays on the machine. The **only** things that leave are calls to configured LLM/embedding providers (Anthropic, optionally OpenAI/Gemini/Ollama). This is the local-first invariant and it is sacred (§4).

---

## 2. Where you are — the map

Five layers. Two share-nothing module systems (don't cross them).

| Layer | Tech | Module system | Size (approx) |
|---|---|---|---|
| **Client** | React 18 + Vite + Tailwind, ProseMirror, Recharts/D3 | **ESM** (`import`) | ~94 files |
| **Server** | Node + Express, `better-sqlite3` (synchronous) | **CommonJS** (`require`) | ~197 files |
| **Desktop** | Electron wrapper | — | 2 files |
| **Mobile** | React Native / Expo | — | ~14 files |
| **Root** | orchestration scripts, env, docs | — | a few |

> Server file counts in older docs say "~133" — the repo has grown to ~197 server / ~311 total. **Trust the live tree, not the doc count.**

### Server anatomy (where the real logic lives)
```
server/
├── index.js            # Express app: routers, auth, rate-limit, scheduler, boot tasks
├── middleware/         # auth (bearer, constant-time, localhost bypass), rateLimit, validate (Zod)
├── lib/                # config, db (sqlite singleton), db-schema, llm, embeddings,
│                       # retrieval, storage, push, event buses, scheduler
├── collectors/         # paste · url · code · email · youtube · file
├── engine/             # intake → parse → fast-path(FTS5) → vault-writer → enrichment
│                       # + context-assembler, grounding-checker, synthesis, dataset-extractor
├── intelligence/       # entities, decisions, graph, patterns, insights, cognitive,
│                       # timeline, proactive  (each: processDocument → extract → store)
├── people/             # extractor, resolver, sentiment, card-generator
├── mcp/                # registry, tool-router, encrypted creds, ~18 connectors
├── operations/         # map · distill · optimize · audit · export
├── features/           # ambient (clipboard/fs watch), actions, briefing, provenance,
│                       # reliability, standing, voice, writing
├── routes/             # 21 Express routers (SSE for chat/ingest/library streams)
└── tests/              # jest; in-memory DB harness in helpers/memDb.js
```

### Client anatomy
```
client/src/
├── pages/        # Dashboard, Library, Inbox, Collect, Search, Settings, People, Writing, ...
├── components/   # chat/ · collectors/ · documents/ · layout/ · library/ · sideroom/ · ui/
├── hooks/        # useChat, useDocuments, useLibraries, useIntelligence, useSearch, useMCP, ...
└── api/client.js # fetch wrapper (GET/POST/PUT/DELETE/postForm)
```

**Rule:** the inventory above is a *map, not a substitute for reading the file*. Before editing any module, open it and read its real exports and call sites. Do not infer a signature — `CLAUDE.md` §3 lists the ones you must not guess.

---

## 3. Your job here — scoped tight

You were brought in for **two things**, in this priority order:

### A. Fix bugs
Targeted, smallest-diff fixes to confirmed defects and regressions.
- Reproduce or precisely locate the defect first. No speculative "fixes."
- Fix the **cause**, not the symptom. Don't paper over with a try/catch that swallows.
- Touch only what the fix requires. Don't drag in a refactor.

### B. Refactor / harden
Improve correctness, structure, reliability, performance, and security of code that already exists.
- Behavior-preserving by default. If a refactor changes behavior, say so loudly and get sign-off.
- Prefer many small, verifiable steps over one big restructure.
- "Harden" means: guard unguarded parses, validate inputs, close race conditions, tighten error handling, remove footguns — *not* add features.

**That's the whole mandate.** Everything else is out of scope unless the owner explicitly expands it.

---

## 4. Hard guardrails — do NOT cross these

1. **Local-first invariant.** Only configured LLM/embedding provider calls may leave the machine. **Never** add telemetry, analytics, crash reporting, or any outbound call to any other host. No exceptions, no "just for debugging."
2. **Write-gating.** Any MCP connector **write** (send email, create issue, post message, write a file to an external store) must require explicit user confirmation in the calling flow. **Never auto-execute a write tool.** Read operations are fine.
3. **No new product surface.** No new collectors, connectors, pages, or features unless explicitly asked. You're hardening what exists (§3).
4. **No multi-tenancy / auth-for-many / billing.** That's "product later," not now (§0).
5. **Don't cross module systems.** Server is CommonJS, client is ESM. Never `import` in `/server` or `require` in `/client/src`.
6. **No TypeScript.** Repo is pure JS (`.js` server, `.js`/`.jsx` client). Don't introduce `.ts`/`.tsx`.
7. **Don't swap core infra.** No new DB driver (it's synchronous `better-sqlite3`), no second test runner, no new state library, no async wrappers around sync DB calls.
8. **No destructive migrations.** Schema changes go through the idempotent `addIfMissing` / `CREATE TABLE IF NOT EXISTS` pattern in the canonical migration path. Never assume a column exists; never write a destructive `ALTER`.
9. **No invented facts.** Do not invent table names, columns, function signatures, or file paths. **Grep first.** If you can't verify it against real source, say so.
10. **No anti-cheat test moves.** Never weaken an assertion, mock the function under test, `skip`/comment-out/delete a failing test, or add `expect(true).toBe(true)` filler to reach green. A green run obtained that way is a *failed task*. (Full rules: `CLAUDE.md` §7.)

---

## 5. What you CAN do freely

- Read anything. Grep, trace call sites, map data flow.
- Fix the confirmed bugs in §7 (and any new ones you can prove).
- Add **real** tests — integration journeys, security-critical units (auth, validate, mcp creds, write-gating), route coverage. Coverage here is a high-value gap.
- Harden error handling, guard `JSON.parse` of any model/DB/connector output, close races, validate inputs at boundaries.
- Refactor for clarity/structure **behavior-preserving**, in small steps, matching existing patterns.
- Improve performance where you can measure it.
- Leave a `// FIXME(harness):` marker when you spot a real defect that's out of your current scope.

---

## 6. Conventions cheat-sheet (full version in `CLAUDE.md`)

- Server = **CommonJS** · Client = **ESM**
- DB = `better-sqlite3`, **synchronous** prepared statements — no async around DB calls
- New IDs = `crypto.randomUUID()` (not the legacy `uuid` import)
- Migrations = idempotent `addIfMissing(col, def)` inside `runMigrations(db)`; new tables `CREATE TABLE IF NOT EXISTS`
- LLM option is **`system`**, NOT `systemPrompt` — passing `systemPrompt` silently drops the prompt
- Don't hardcode a model name in feature code — call `llm.complete()` / `llm.stream()`; let `lib/llm.js` resolve it
- Datasets from `getByLibrary()` arrive **pre-parsed** (data already `JSON.parse`d)
- Tests = `jest`, `testEnvironment: node`, 30s timeout, in-memory DB via `tests/helpers/memDb.js` + `resetDb()` in `beforeEach`
- SSE endpoints: `res.write('event: X\ndata: ...\n\n')`; always clean up on `req.on('close')`

---

## 7. Known weak spots (verify against live source before fixing)

These were triaged in `CLAUDE.md` §5. Confirm each still exists before touching it — the code may have moved on.

1. **VIZ regex can't parse nested JSON** — `routes/chat.js` uses `/\[VIZ:(\{[^}]+\})\]/g`; `[^}]+` stops at the first `}`, breaking any payload with a nested object. Use a balanced capture + `JSON.parse` in try/catch; validate `datasetId` before emitting.
2. **VIZ blocks split across SSE tokens** — buffer until a complete `[VIZ:...]` block is present; dedupe so it's emitted once and never half-formed.
3. **`systemPrompt` vs `system` mismatch** — grep `/server` for `systemPrompt`; the real signature is `{ system }`.
4. **Fire-and-forget dataset extraction** — `enrichment.runAll` kicks off extraction without awaiting or emitting a done event; downstream can race. Await it or emit an `ingest-bus` completion event.
5. **Unguarded `JSON.parse` kills chat** — at least one chat path parses model/DB JSON without try/catch; one bad payload 500s the request. Wrap every parse of model/DB/connector output with a safe fallback.

> If you fix one, also scan for sibling instances of the same class of bug — these are patterns, not one-offs.

---

## 8. Definition of done (every task)

A task is **not done** if the app doesn't boot or tests don't actually pass.

1. **Restate** the task in one line and name the exact files you'll touch.
2. **Grep/read** their real exports and call sites before editing.
3. **Smallest diff** that satisfies the task; match surrounding style.
4. **Verify for real:**
   - App boots: `node -e "require('./index.js')"` (from `server/`) throws nothing.
   - `npm test` (server) actually passes — paste the real output, don't claim it.
5. **Report honestly:** what changed, what you verified, what you *assumed*, what's still untested or broken. State which tests are real vs. stubs. Flag any §7 bug you touched. If something can't be done correctly, **stop and say so** — don't fake green.

---

## 9. First-hour checklist for a new model

1. Read this file, then `CLAUDE.md` fully.
2. Get the app to boot locally (`npm run install:all`, then `npm run dev` or `node -e "require('./index.js')"` in `server/`).
3. Run the test suite and read the output — know your baseline before you change anything.
4. Pick the **smallest** real task. Reproduce/locate the issue against live source.
5. Make the smallest diff, verify it boots + tests pass, report honestly.
6. When unsure whether something is in scope: re-read §0 and §3. If still unsure, ask — don't guess.

---

*This harness is the map and the rules. `CLAUDE.md` is the build manual. The live source is the truth. When in doubt, read the code and ask the owner.*

---

## 10. CURRENT CONFIGURATION CONTRACT (2026-07-18 — supersedes older docs)

The runtime was overhauled on 2026-07-18. **Do not "reconfigure" any of the
following back — several older values look plausible but are dead.**

### LLM configuration (`server/data/config.json` — gitignored, holds keys)
- **Chat / interactive:** provider `openai`, model **`gpt-5-chat-latest`**.
- **Knowledge work (structured extraction):** `knowledgeWork` = openai
  **`gpt-5-mini`** (verified). Backup: `knowledgeWorkBackup` = mistral
  `mistral-small-latest`.
- **`gpt-5.6-terra` is DEAD — the key 401s on it. Never set it anywhere.**
  A stale agent cache restored it once already and silently broke enrichment.
- **Ollama / localModels: intentionally OFF** (`localModels.enabled: false`)
  to keep resources down. Do not re-enable, do not spawn `ollama serve`.
- **Neo4j: intentionally OFF** (`graph.neo4j.enabled: false`). SQLite is the
  graph source of truth; do not start the Java runtime.
- **Budgets are ceilings, not targets** (`paidAiBudget`): 240 req/hr,
  2000/day, background 180/hr / 1200/day. They pace the re-enrichment sweep.
  Do not lower them without asking the owner; raising ends the sweep sooner.
- `lib/config.js` **saveConfig merges against DISK, not memory** — never
  bypass it with a whole-file write from a cached copy.

### Autonomous loops (all config-gated; owner wants them ON)
- `refinement.autoAfterIngest: true` — refinement fires itself ~90s after
  ingestion settles; CDC-gated so unchanged corpora cost one indexed lookup.
- `refinement.autoAllowPaidAi: true`, `refinement.autoResumePaid: true` —
  paid passes run and resume with no human input; budget ledger is the guard.
- `enrichment.autoResweep: true` — the sweeper upgrades documents whose
  `documents.enrichment_version < 2` (pipeline v2 = gpt-5-mini + spaCy +
  meta-embeddings). ~3.7k-doc backlog draining through ~mid-Aug 2026. It only
  uses idle enrichment-queue capacity; live ingestion always wins.
- Post-sweep one-shot VACUUM via `maintenance_flags` at next boot.

### Schema added 2026-07-18 (additive; do not drop)
`documents.enrichment_version` · `document_meta_embeddings` (summary +
answerable-question vectors, used by retrieval boosts) · `change_log` +
triggers on documents/library_documents/document_sections/document_facts
(CDC — every writer is captured; `lib/change-tracker.js` is the cursor API)
· `maintenance_flags` · `retrieval_eval_sets/runs` (eval harness).

### Runtime facts
- Server boots via ts-node **with SWC** (`tsconfig.json` `ts-node.swc: true`,
  needs devDep `@swc/core`). Do not remove; do not switch to compiled `dist/`
  output — `__dirname`-relative paths (data dir, `lib/py/` workers) break.
- Python 3.12 at `%LOCALAPPDATA%\Programs\Python\Python312` provides Docling
  (layout-aware parsing, preferred for pdf/docx/pptx/xlsx/images with JS
  fallback), spaCy NER (`en_core_web_sm`, persistent worker), and fastembed
  (free local embeddings fallback). The untrusted-file scraper subprocess
  pins `DOCLING=0` on purpose — leave it.
- Node lives at `%LOCALAPPDATA%\Programs\node-v20.20.2-win-x64` (not on PATH).
- LLM layer has a per-provider circuit breaker, a global background-call
  semaphore (`LLM_BACKGROUND_CONCURRENCY`, default 6), and an LRU cache for
  background completions. Interactive chat is never queued or cached.
- Tests: **mock `lib/config` and `lib/embeddings`** — the eval route defaults
  to lexical mode under jest precisely so unmocked suites can't hit live
  providers. Suite baseline: 2264 server tests green; keep it green.

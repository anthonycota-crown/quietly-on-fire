# ContextOS — Coding Agent Harness

**Purpose:** This file is the operating contract for any coding agent (Claude Code, etc.) working on ContextOS. Read it fully before your first edit. It encodes the real architecture, the conventions you must not break, the confirmed bugs, and the anti-cheat rules for test loops. When this file and a design doc disagree, **the live source code wins**, then this file, then the design docs (docs lag the code by ~1 day).

---

## 0. Non-Negotiables (read first)

- **No bullshit.** If you can't verify something against real source, say so. Do not invent table names, column names, function signatures, or file paths. Grep first.
- **Local-first invariant.** Only Anthropic API calls (and configured LLM/embedding providers) leave the machine. Never add telemetry, analytics, or outbound calls to any other host.
- **Read before write.** Before editing any module, `grep` its real exports and call sites. The inventory in this file is a map, not a substitute for reading the file.
- **Write-gating.** MCP connector **write** operations (send email, create issue, post message, write file) require explicit user confirmation in the calling flow. Read operations are low-risk. Never auto-execute a write tool.
- **Child of an existing system.** ~133 server source files already exist and mostly work. You are extending and fixing, not greenfielding. Match existing patterns exactly.

---

## 1. Stack & Hard Conventions

| Concern | Convention | Do NOT |
|---|---|---|
| Server module system | **CommonJS** (`require` / `module.exports`), `"type": "commonjs"` | Use `import`/`export` in `/server` |
| Client module system | **ESM** (`"type": "module"`), React 18 + Vite | Use `require` in `/client/src` |
| DB | `better-sqlite3`, **synchronous** prepared statements | Use async/await around DB calls; use any other driver |
| IDs | `crypto.randomUUID()` | Use `uuid` v4 import for new IDs (legacy `uuid` dep exists but standardize on `crypto`) |
| Migrations | `addIfMissing(col, def)` idempotent pattern in `runMigrations(db)` (see §4) | Write destructive `ALTER`; assume a column exists |
| File types | Server `.js` only, Client `.js`/`.jsx` only | Introduce TypeScript (`.ts`/`.tsx`) — repo is pure JS |
| Tests | `jest`, `testEnvironment: node`, 30s timeout | Add a second test runner |

**Default LLM model string for in-app Anthropic calls:** match whatever `lib/llm.js` already resolves from config. Do not hardcode a model name in feature code; call `llm.complete()` / `llm.stream()`.

---

## 2. Repo Map (ground truth, ~133 server files)

```
server/
├── index.js                 # Express app; registers all routers, auth, rate-limit,
│                            # scheduler, intelligence.init(), mcp.restore(), ambient,
│                            # reclaimOrphans() on boot
├── middleware/
│   ├── auth.js              # authMiddleware — bearer, constant-time, localhost bypass
│   ├── rateLimit.js         # apiLimiter (300/min), strictLimiter (60/min: ingest, chat)
│   └── validate.js          # validateBody(schema) + Zod schemas.*
├── lib/
│   ├── config.js            # getConfig/saveConfig/loadConfig/resetConfig -> data/config.json
│   ├── db.js                # better-sqlite3 singleton; WAL + FK; applies schema + migrations
│   ├── db-schema.js         # applyBaseSchema(db), runMigrations(db)
│   ├── embeddings.js        # embed/similarity/serialize; OpenAI+Ollama only
│   ├── llm.js               # complete/stream/vision/supportsVision/testConnection
│   ├── retrieval.js         # retrieveTopChunks (TF-IDF)
│   ├── storage.js           # readDocument/writeDocument/... local|github|obsidian
│   ├── push.js              # web-push VAPID
│   ├── ingest-bus.js        # emit/subscribe per documentId (stage/status/detail/error)
│   ├── library-bus.js       # emit/subscribe per libraryId (conflict/gap/synthesis)
│   └── scheduler.js         # node-cron start/loadJobs
├── collectors/              # paste, url, code, email, youtube, file
├── engine/
│   ├── index.js             # ingest/getStatus/assembleContext/groundResponse/reEnrich/...
│   ├── intake.js            # dup-check -> media -> parse -> fast-path -> vault -> enrich
│   ├── parser.js            # parse(md)->Section[]; wordCount
│   ├── fast-path.js         # FTS5 index + retrieval chunks
│   ├── duplicate-detector.js# SHA-256 check
│   ├── vault-writer.js      # readable.md/source.*/digest.md/meta
│   ├── context-assembler.js # assemble(query, libraryId, history)
│   ├── grounding-checker.js # ground(...)
│   ├── enrichment.js        # runAll: summaries/facts/class/keysec/embeddings/digest/datasets
│   ├── synthesis.js         # run(libraryId)->{intent,gaps,themes}
│   ├── fact-retriever.js    # getRelevantFacts/getConflicts
│   ├── section-retriever.js # embeddings w/ FTS fallback
│   ├── query-rewriter.js
│   ├── conversation-compressor.js
│   ├── dataset-extractor.js # isDataCandidate/extractDatasets/getByDocument/getByLibrary
│   ├── errors.js            # EngineError/makeError/classify*
│   ├── events.js            # bus: document:complete|deleted|deleting
│   ├── stores/              # registry, sections, chunks, summaries, facts
│   └── media/               # index, visual(OCR/vision), code, temporal, structured
├── intelligence/
│   ├── index.js             # init(): schema + subscribe document:complete; runs extractors
│   ├── schema.js
│   └── {entities,decisions,graph,patterns,insights,cognitive,timeline,proactive}/
│       # each: index.processDocument, extractor.extract, store.insert/getBy*
├── people/                  # store, extractor, resolver, sentiment-tracker,
│                            # card-generator, index.processDocument
├── mcp/
│   ├── registry.js          # register/get/list/restore
│   ├── connector-interface.js
│   ├── tool-router.js       # callTool/listAllTools
│   ├── db.js                # save/load/deleteCredentials (encrypted)
│   └── connectors/          # 18: gmail, google-drive, google-calendar, github, notion,
│                            #     slack, apple-notes, apple-reminders, linear, obsidian,
│                            #     readwise, spotify, telegram, youtube, plaid, ical,
│                            #     openai, anthropic
├── operations/              # map, distill, optimize, audit, export
├── features/
│   ├── ambient/             # clipboard-watcher, filesystem-watcher
│   ├── actions/             # detector, executor
│   ├── briefing/generator.js
│   ├── provenance/          # citation-mapper, verifier
│   ├── reliability/index.js
│   ├── standing/index.js
│   ├── voice/transcriber.js
│   └── writing/context-lookup.js
├── routes/                  # 21 routers (see §6)
└── tests/
    ├── intake.smoke.test.js # the ONE real test
    └── helpers/memDb.js     # in-memory SQLite w/ prod schema+migrations; exports db, resetDb()

client/src/  # 57 .jsx/.js — App, pages/, components/{chat,collectors,documents,
             # layout,library,sideroom,ui}, hooks/, api/client.js
```

---

## 3. Critical Function Signatures (do not guess these)

```js
// lib/llm.js  — NOTE the option name is `system`, NOT `systemPrompt`
llm.complete(messages, { maxTokens, system, provider, model, timeout, preferLocal, cacheSystem })
llm.stream(messages, { /* same options */ })   // AsyncGenerator<string>
llm.vision(imageBuffer, mimeType, prompt, options)

// engine/index.js
engine.ingest(rawContent, collectorType, metadata, libraryId?)
  // -> { documentId, status, title, wordCount, sectionCount }
engine.assembleContext(query, libraryId, history?)
engine.groundResponse(messageId, responseText, libraryId, contextSections)

// engine/dataset-extractor.js
getByLibrary(libraryId)  // returns Dataset[] with data_json ALREADY PARSED + document_title

// lib/embeddings.js
similarity(queryVec, embeddingBuf)  // 0..1 cosine

// people/store.js  — IDs returned are strings
upsertPerson({ canonical_name, aliases, bio, role, organization, email, linkedin }) // -> id

// mcp/tool-router.js
callTool(toolName, params, context?)  // write tools require user confirmation upstream
```

If a signature you need is not listed here, **open the file and read it** before calling.

---

## 4. Database & Migration Pattern (copy exactly)

All new columns go through the idempotent helper inside `runMigrations(db)` in `lib/db-schema.js`:

```js
const existingCols = db.prepare(`PRAGMA table_info(documents)`).all().map(c => c.name);
const addIfMissing = (col, definition) => {
  if (!existingCols.includes(col)) {
    db.prepare(`ALTER TABLE documents ADD COLUMN ${col} ${definition}`).run();
  }
};
addIfMissing('my_new_col', `TEXT DEFAULT NULL`);
```

For a **new table**, guard with `CREATE TABLE IF NOT EXISTS`. New IDs: `crypto.randomUUID()`. Timestamps: integer epoch ms unless the surrounding table already uses something else — **match the neighboring columns**.

**Known structural gap:** there is no single migration ledger / `schema_migrations` table. Migrations are scattered across `db-schema.js`, `intelligence/schema.js`, and inline in some modules. When you add schema, register it in the canonical `runMigrations` path, not inline in a feature file, unless you are matching an existing local pattern in that subsystem.

**Test DB:** use `tests/helpers/memDb.js` (`require` it, call `resetDb()` in `beforeEach`). It runs the same schema + migrations in-memory. Never point a test at the real `data/` DB.

---

## 5. Confirmed Bugs (fix carefully; do not regress)

These are real defects already triaged. When you touch the surrounding code, fix them; otherwise leave a `// FIXME(harness):` if out of scope.

1. **VIZ regex can't parse nested JSON.**
   In `routes/chat.js` the pattern is `/\[VIZ:(\{[^}]+\})\]/g`. `[^}]+` stops at the first `}`, so any VIZ payload containing a nested object (e.g. a non-null `filterCategory`) fails to match. Fix: use a balanced/greedy capture that tolerates one level of nesting, then `JSON.parse` inside try/catch. Validate `datasetId` against `libraryDatasets` before emitting.

2. **VIZ blocks split across SSE tokens.**
   The handler runs `matchAll` on `fullText` per token, which is correct for *detection* but the dedupe key is `match[0]` (raw). Confirm a block that arrives across two tokens is only emitted once and is never emitted half-formed. Buffer until a complete `[VIZ:...]` is present before parsing.

3. **`llm` option naming mismatch.**
   The Visualization design doc calls `llm.stream(messages, { systemPrompt })`. The real signature is `{ system }`. Any code using `systemPrompt` silently drops the system prompt. Grep for `systemPrompt` in `/server` and pass `system`.

4. **Fire-and-forget dataset extraction has no completion signal.**
   `enrichment.runAll` kicks off `dataset-extractor.extractDatasets` without awaiting or emitting a done event. Downstream (VIZ, library datasets) can race. Fix: await it, or emit an `ingest-bus` event (`stage: 'datasets', status: 'complete'`) the client/synthesis can wait on.

5. **Unguarded `JSON.parse` breaks chat.**
   At least one chat path parses LLM/DB JSON without try/catch; a malformed payload throws and kills the stream. Wrap every `JSON.parse` of model/DB/connector output in try/catch with a safe fallback. Never let one bad parse 500 the request.

---

## 6. Routes (21 routers, all export Express `Router`)

`libraries, documents, collect, chat, search, system, intelligence, operations, synthesis, notifications, export, briefing, voice, ambient, actions, writing, people, standing, notices, mcp, settings`

- `strictLimiter` guards expensive routes (ingest, chat). Keep it.
- SSE endpoints (`documents` progress, `libraries` events, `chat` stream) use `res.write('event: X\ndata: ...\n\n')`. Always end with proper cleanup on `req.on('close')`.
- No route has unit tests. Adding integration tests against routes is high value (see §7).

---

## 7. Test Loop & Anti-Cheat Rules

**Current coverage: effectively <1% (one real smoke test).** Priority order when asked to add tests:

1. **Step 0 — Static sweep.** Make the app boot. `node -e "require('./index.js')"` must not throw. Resolve missing requires, undefined exports, broken migrations.
2. **Step 1 — Integration tests.** Prove end-to-end journeys work against the in-memory DB: ingest a paste → sections+FTS created; chat round-trip assembles context; a connector `listTools()` returns shape.
3. **Step T1 — Security-critical units.** `auth.js` (constant-time, localhost bypass, 401 path), `validate.js` schemas (reject bad bodies), `mcp/db.js` (encrypt/decrypt round-trip), connector write-gating.
4. Broader unit coverage after the above.

### Anti-cheat (these are hard rules — a green run obtained by any of these is a FAILED task)

- **Never** weaken an assertion to make it pass. If reality differs from the expected value, report the discrepancy; do not edit the expectation to match buggy output.
- **Never** mock the function under test. Mock its *dependencies* (LLM, network, vault, embeddings) only.
- **Never** wrap the test body in `try {} catch {}` that swallows failures.
- **Never** `it.skip` / `test.skip` / `xit` / comment-out a failing test to reach green.
- **Never** add `expect(true).toBe(true)` filler or assert on a value you just hardcoded.
- **Never** delete a failing test instead of fixing the cause.
- If a test legitimately cannot pass because the underlying code is broken, **fix the code or stop and report** — do not paper over it.
- Mocks must reflect the real return *shape* (see §3). A mock that returns a shape the real function never returns is a cheat.

State explicitly in your summary: which tests are real, which are stubs, and what is still uncovered.

---

## 8. Working Agreement (per task)

1. **Restate** the task in one line and name the exact files you'll touch.
2. **Grep/read** those files' real exports and call sites before editing.
3. **Smallest diff** that satisfies the task. Match surrounding style (CommonJS, sync DB, `crypto.randomUUID`).
4. **Run** `npm test` (server) and `node -e "require('./index.js')"`. Report real output, not a claim.
5. **Summarize** honestly: what changed, what you verified, what you assumed, what's still untested or broken. Flag any of the §5 bugs you touched.

Do not mark a task done if the app doesn't boot or tests don't actually pass.

---

## 9. Quick Reference Card

- Server = CommonJS · Client = ESM
- DB = better-sqlite3, **sync**, `addIfMissing` migrations
- IDs = `crypto.randomUUID()`
- LLM option = `system` (not `systemPrompt`)
- Datasets from `getByLibrary()` come **pre-parsed**
- MCP writes = user-confirmed only
- Tests = jest + `tests/helpers/memDb.js` + `resetDb()`
- Only Anthropic/provider calls leave the machine
- Docs lag code ~1 day; **live source is truth**

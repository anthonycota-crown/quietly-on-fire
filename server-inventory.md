# ContextOS Server — JS File Inventory

**Total source files (excl. node_modules):** ~168 `.js` files across 13 directories  
**Test files:** 2 (coverage noted per module below)

---

## Tests Directory

| File | What it covers |
|------|---------------|
| `tests/intake.smoke.test.js` | Engine intake pipeline — mocks LLM, vault-writer, storage, embeddings; asserts documents → sections → FTS all created correctly |
| `tests/helpers/memDb.js` | Shared in-memory SQLite DB (same schema + migrations as prod); exports `db`, `resetDb()` |

> **Coverage gap:** No tests exist for collectors, routes, middleware, features, intelligence extractors, operations, people, or MCP connectors.

---

## `index.js` (root)

| Export | Inputs | Output |
|--------|--------|--------|
| `app` | — | Express application instance |

Registers all routers, initialises CORS/auth/rate-limiting, starts push notifications, scheduler, intelligence layer, MCP sessions, ambient capture, and reclaims orphaned documents on startup.

**Test:** ✗

---

## Middleware (`/middleware/`)

### `auth.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `authMiddleware` | `(req, res, next)` | Calls `next()` or sends 401 |

Bearer token auth with constant-time comparison; bypasses for localhost.

**Test:** ✗

---

### `rateLimit.js`
| Export | Config |
|--------|--------|
| `apiLimiter` | 300 req/min — authenticated API routes |
| `strictLimiter` | 60 req/min — expensive endpoints (ingest, chat) |

**Test:** ✗

---

### `validate.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `validateBody(schema)` | Zod schema | Express middleware that validates `req.body` |
| `schemas.libraryCreate` | — | Zod schema |
| `schemas.libraryUpdate` | — | Zod schema |
| `schemas.collectPaste` | — | Zod schema |
| `schemas.collectUrl` | — | Zod schema |
| `schemas.collectYoutube` | — | Zod schema |
| `schemas.collectEmail` | — | Zod schema |
| `schemas.chatMessage` | — | Zod schema |

**Test:** ✗

---

## Core Libraries (`/lib/`)

### `config.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `getConfig()` | — | Config object |
| `saveConfig(updates)` | `updates: object` | Persists to `data/config.json` |
| `loadConfig()` | — | Loads file or applies defaults |
| `resetConfig()` | — | Clears in-memory cache |

Config shape: `{ llm, storage, notifications, localModels, ambientCapture }`

**Test:** ✗

---

### `db.js`
| Export | Output |
|--------|--------|
| `db` | `better-sqlite3` Database singleton |

Applies base schema + migrations; enables WAL mode + foreign keys.

**Test:** ✗

---

### `db-schema.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `applyBaseSchema(db)` | `db: Database` | Creates 30+ base tables |
| `runMigrations(db)` | `db: Database` | Adds columns + new tables (sections, facts, summaries, datasets, MCP connectors, standing queries, people, actions, writing, document_datasets) |

**Test:** ✗

---

### `embeddings.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `supportsEmbeddings()` | — | `boolean` |
| `getEmbedModel()` | — | `string \| null` |
| `embed(texts)` | `texts: string[]` | `Promise<Float32Array[]>` (normalized) |
| `serializeEmbedding(vec)` | `vec: Float32Array` | `Buffer` |
| `deserializeEmbedding(buf)` | `buf: Buffer` | `Float32Array` |
| `similarity(queryVec, embeddingBuf)` | `queryVec: Float32Array`, `embeddingBuf: Buffer` | `number` (0–1 cosine similarity) |

Providers: OpenAI, Ollama (Anthropic + Google don't expose embedding APIs).

**Test:** ✗

---

### `llm.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `complete(messages, options?)` | `messages: Message[]`, `options?: { maxTokens, system, provider, model, timeout, preferLocal, cacheSystem }` | `Promise<string>` |
| `stream(messages, options?)` | same as above | `AsyncGenerator<string>` (text tokens) |
| `vision(imageBuffer, mimeType, prompt, options?)` | `imageBuffer: Buffer`, `mimeType: string`, `prompt: string`, `options?` | `Promise<string>` |
| `supportsVision()` | — | `boolean` |
| `testConnection(overrideConfig?)` | `overrideConfig?: object` | `Promise<{ ok: boolean, error?: string }>` |

Providers: Anthropic, OpenAI, Google, Groq, Ollama, OpenAI-compatible.

**Test:** ✗

---

### `retrieval.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `retrieveTopChunks(query, chunks, k?)` | `query: string`, `chunks: Chunk[]`, `k?: number = 20` | `string[]` (chunk IDs sorted by TF-IDF relevance) |

**Test:** ✗

---

### `storage.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `readDocument(id)` | `id: string` | `Promise<{ content, meta }>` |
| `writeDocument(id, content, meta)` | `id: string`, `content: string`, `meta: object` | `Promise<void>` |
| `deleteDocument(id)` | `id: string` | `Promise<void>` |
| `listDocuments()` | — | `Promise<string[]>` |
| `testBackend()` | — | `Promise<{ ok: boolean }>` |

Backends: local filesystem, GitHub, Obsidian (configured via `config.json`).

**Test:** ✗

---

### `push.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `init()` | — | Sets up VAPID for web-push |
| `sendToAll(payload)` | `payload: object` | `Promise<void>` |
| `configured()` | — | `boolean` |

**Test:** ✗

---

### `ingest-bus.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `emit(documentId, event)` | `documentId: string`, `event: object` | `void` |
| `subscribe(documentId, fn)` | `documentId: string`, `fn: (event) => void` | `() => void` (unsubscribe) |

Events: `{ stage, status, detail, error }` progress updates during document processing.

**Test:** ✗

---

### `library-bus.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `emit(libraryId, event)` | `libraryId: string`, `event: object` | `void` |
| `subscribe(libraryId, fn)` | `libraryId: string`, `fn: (event) => void` | `() => void` (unsubscribe) |

Events: `conflict_alert`, `gap_alert`, `synthesis_complete`.

**Test:** ✗

---

### `scheduler.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `start()` | — | Starts cron for background jobs |
| `loadJobs()` | — | Loads enabled jobs from database |

**Test:** ✗

---

## Collectors (`/collectors/`)

### `paste.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `collectPaste(text, title?)` | `text: string`, `title?: string` | `{ title, source, collector_type, content_md, language }` |

Auto-generates title; normalizes markdown/plain text; detects ALL CAPS headings.

**Test:** ✗

---

### `url.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `collectUrl(url)` | `url: string` | `Promise<document object>` |

SSRF protection (blocks private IPs); extracts og: metadata; removes noise (ads, nav, footer); HTML → Markdown via Turndown.

**Test:** ✗

---

### `code.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `collectCode(file)` | `file: { name, content, path }` | `document object` |

Detects 45+ extensions/languages; extracts comments, docstrings, function/class definitions for large files (>500 lines).

**Test:** ✗

---

### `email.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `collectEmail(rawEmail)` | `rawEmail: string` | `document object` |

Parses MIME headers (From, To, CC, Date, Subject); separates body from quoted replies; handles quoted-printable encoding.

**Test:** ✗

---

### `youtube.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `collectYoutube(url)` | `url: string` | `Promise<document object>` |

Fetches transcript via `youtube-transcript` package; extracts metadata.

**Test:** ✗

---

### `file.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `collectFile(buffer, filename, mimeType)` | `buffer: Buffer`, `filename: string`, `mimeType: string` | `Promise<document object>` |

Handles PDF, DOCX, XLSX, TXT, HTML, CSV, images (OCR via Tesseract), code files.

**Test:** ✗

---

## Engine (`/engine/`)

### `index.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `ingest(rawContent, collectorType, metadata, libraryId?)` | `rawContent: string`, `collectorType: string`, `metadata: object`, `libraryId?: string` | `Promise<{ documentId, status, title, wordCount, sectionCount }>` |
| `getStatus(documentId)` | `documentId: string` | `object` (document status) |
| `assembleContext(query, libraryId, history?)` | `query: string`, `libraryId: string`, `history?: Message[]` | `Promise<context object>` |
| `getVaultPath(documentId)` | `documentId: string` | `string` |
| `getReadable(documentId)` | `documentId: string` | `Promise<string>` (markdown) |
| `groundResponse(messageId, responseText, libraryId, contextSections)` | strings + array | `Promise<grounding object>` |
| `reEnrich(documentId)` | `documentId: string` | `Promise<void>` |
| `deleteDocument(documentId)` | `documentId: string` | `Promise<void>` |
| `reclaimOrphans()` | — | `Promise<void>` |
| `runSynthesis(libraryId)` | `libraryId: string` | `Promise<void>` |

**Test:** ✓ (`intake.smoke.test.js` covers the intake portion)

---

### `errors.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `EngineError` | — | Custom error class |
| `makeError(code, detail?)` | `code: string`, `detail?: string` | `EngineError` |
| `classifyHTTPError(status)` | `status: number` | `string` (error code) |
| `classifyNetworkError(err)` | `err: Error` | `string` (error code) |

Error codes: `PAYWALL`, `TIMEOUT`, `JS_RENDERED`, `BLOCKED`, `DUPLICATE_EXACT`, `DUPLICATE_SOURCE`, `AI_UNAVAILABLE`, etc.

**Test:** ✗

---

### `events.js`
| Export | Output |
|--------|--------|
| `bus` | `EventEmitter` |

Emits: `document:complete`, `document:deleted`, `document:deleting`.

**Test:** ✗

---

### `parser.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `parse(markdownContent)` | `markdownContent: string` | `Section[]` |
| `wordCount(text)` | `text: string` | `number` |

Section shape: `{ id, index, heading, headingLevel, precedingHeading, type (code/table/quote/list/prose), content, contentMarkdown, wordCount, position (0–1) }`. Falls back to paragraph-splitting if fewer than 2 headings.

**Test:** ✗

---

### `fast-path.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `run(documentId, sections)` | `documentId: string`, `sections: Section[]` | `Promise<void>` |

Indexes into FTS5 + creates retrieval chunks.

**Test:** ✗

---

### `duplicate-detector.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `check(contentHash)` | `contentHash: string` | `{ isDuplicate: boolean, existingId?: string, existingTitle?: string }` |

SHA-256 content hash comparison.

**Test:** ✗

---

### `vault-writer.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `write(documentId, { sourceBuffer?, sourceExt?, cleanContent, meta })` | `documentId: string`, options object | `Promise<vault object>` |
| `readReadable(documentId)` | `documentId: string` | `Promise<string>` (markdown) |
| `deleteVault(documentId)` | `documentId: string` | `Promise<void>` |
| `updateMeta(documentId, meta)` | `documentId: string`, `meta: object` | `Promise<void>` |

Writes: `readable.md`, `source.*`, `digest.md`, metadata JSON.

**Test:** ✗

---

### `context-assembler.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `assemble(query, libraryId, history?)` | `query: string`, `libraryId: string`, `history?: Message[]` | `Promise<{ sections, facts, datasets, entities, summary }>` |

**Test:** ✗

---

### `grounding-checker.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `ground(messageId, responseText, libraryId, contextSections)` | strings + array | `Promise<{ groundedClaims, ungroupedClaims, score }>` |

**Test:** ✗

---

### `enrichment.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `runAll(documentId, libraryId?)` | `documentId: string`, `libraryId?: string` | `Promise<void>` |

Stages: summaries, facts, classification, key sections, embeddings, digest, datasets (fire-and-forget).

**Test:** ✗

---

### `synthesis.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `run(libraryId)` | `libraryId: string` | `Promise<{ intent, gaps, themes }>` |

**Test:** ✗

---

### `intake.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `intake(rawContent, collectorType, metadata, libraryId?)` | `rawContent: string`, `collectorType: string`, `metadata: object`, `libraryId?: string` | `Promise<{ documentId, title, wordCount, sectionCount }>` |

Coordinates: duplicate check → media processing → section parsing → fast-path → vault write → background enrichment.

**Test:** ✓ (`intake.smoke.test.js`)

---

### `fact-retriever.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `getRelevantFacts(query, libraryId, limit?)` | `query: string`, `libraryId: string`, `limit?: number` | `Fact[]` |
| `getConflicts(libraryId)` | `libraryId: string` | `ConflictPair[]` |

**Test:** ✗

---

### `section-retriever.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `retrieve(query, libraryId, k?)` | `query: string`, `libraryId: string`, `k?: number` | `Promise<Section[]>` |

Semantic retrieval via embeddings; falls back to FTS if embeddings unavailable.

**Test:** ✗

---

### `query-rewriter.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `rewrite(query, history?)` | `query: string`, `history?: Message[]` | `Promise<string>` |

**Test:** ✗

---

### `conversation-compressor.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `compress(messages, maxTokens?)` | `messages: Message[]`, `maxTokens?: number` | `Promise<{ summary, messages }>` |

**Test:** ✗

---

### `dataset-extractor.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `isDataCandidate(section)` | `section: Section` | `boolean` (requires ≥2 of 7 heuristic signals) |
| `extractDatasets(documentId, sectionIds)` | `documentId: string`, `sectionIds: string[]` | `Promise<void>` (writes to DB) |
| `getByDocument(documentId)` | `documentId: string` | `Dataset[]` (with `data_json` parsed) |
| `getByLibrary(libraryId)` | `libraryId: string` | `Dataset[]` (includes `document_title`) |

Heuristic signals: markdown tables, currency values, comma-separated numerics, date patterns, ≥5 numbers, data-bearing headings, CSV-like structure.

**Test:** ✗

---

## Engine Stores (`/engine/stores/`)

### `registry.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `insert(doc)` | `doc: object` | `void` |
| `updateStatus(id, status, extra?)` | `id: string`, `status: string`, `extra?: object` | `void` |
| `getById(id)` | `id: string` | `document object \| null` |
| `getByHash(hash)` | `hash: string` | `{ id, title } \| null` |

**Test:** ✗

---

### `sections.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `insertAll(documentId, sections)` | `documentId: string`, `sections: Section[]` | `void` |
| `markKeySection(sectionId)` | `sectionId: string` | `void` |
| `getByDocument(documentId)` | `documentId: string` | `Section[]` |

**Test:** ✗

---

### `chunks.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `insertAll(documentId, chunks)` | `documentId: string`, `chunks: Chunk[]` | `void` |
| `getByDocument(documentId)` | `documentId: string` | `Chunk[]` |

**Test:** ✗

---

### `summaries.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `upsert(documentId, scale, text)` | `documentId: string`, `scale: 'micro'\|'short'\|'medium'`, `text: string` | `void` |
| `getByDocument(documentId)` | `documentId: string` | `{ micro, short, medium }` |

**Test:** ✗

---

### `facts.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `insertAll(documentId, facts)` | `documentId: string`, `facts: Fact[]` | `void` |
| `flag(factId, reason)` | `factId: string`, `reason: string` | `void` |
| `getByDocument(documentId)` | `documentId: string` | `Fact[]` |
| `getByLibrary(libraryId)` | `libraryId: string` | `Fact[]` |

**Test:** ✗

---

## Engine Media (`/engine/media/`)

### `index.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `process(documentId, collectorType, buffer?)` | `documentId: string`, `collectorType: string`, `buffer?: Buffer` | `Promise<void>` |

Routes to specialized processor based on collector type.

**Test:** ✗

---

### `visual.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `processVisual(documentId, imageBuffer)` | `documentId: string`, `imageBuffer: Buffer` | `Promise<{ text, meta }>` |

OCR via Tesseract; vision LLM for screenshots.

**Test:** ✗

---

### `code.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `processCode(documentId, content, language)` | `documentId: string`, `content: string`, `language: string` | `Promise<{ structure, comments }>` |

**Test:** ✗

---

### `temporal.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `processTemporal(documentId, content)` | `documentId: string`, `content: string` | `Promise<{ events, timeline }>` |

Timestamp extraction + timeline building.

**Test:** ✗

---

### `structured.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `processStructured(documentId, content)` | `documentId: string`, `content: string` | `Promise<{ tables, datasets }>` |

**Test:** ✗

---

## Routes (`/routes/`)

All route files export an Express `Router`. No unit tests for any routes.

| File | Key Endpoints |
|------|--------------|
| `libraries.js` | CRUD + collect (url/paste/youtube/email/code/file) + inbox move + SSE events + datasets |
| `documents.js` | List/get/update/delete + status/progress SSE + readable + facts + reliability + datasets |
| `collect.js` | POST file, code, url, paste, youtube, email, voice |
| `chat.js` | Sessions CRUD + streaming message POST + grounding GET |
| `search.js` | GET `/?q=` + library filter; FTS5 with LIKE fallback |
| `system.js` | Stats, auth-verify, config CRUD, LLM test, storage test |
| `intelligence.js` | Entities, decisions, patterns, insights, cognitive, timeline |
| `operations.js` | Run/list/get operations (map, distill, optimize, audit, export) |
| `synthesis.js` | GET library synthesis |
| `notifications.js` | Register push token, send test |
| `export.js` | POST export (markdown/PDF/ZIP) |
| `briefing.js` | GET generate daily briefing |
| `voice.js` | List voices, POST synthesize TTS |
| `ambient.js` | Check enabled, GET clipboard |
| `actions.js` | POST detect, POST enrich/:id, POST execute/:id, POST cancel/:id |
| `writing.js` | Sessions CRUD + autosave + context lookup + archive/export + dismiss suggestion |
| `people.js` | List/get people + PUT fulfill commitment + POST regenerate-card |
| `standing.js` | Create/list standing queries + GET answer history |
| `notices.js` | GET system notices |
| `mcp.js` | List connectors, connect, call tool |
| `settings.js` | GET/PUT user settings |

**Tests:** ✗ (all routes)

---

## Features (`/features/`)

### `ambient/clipboard-watcher.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `start()` | — | Starts polling clipboard; emits on change |
| `stop()` | — | Stops polling |

**Test:** ✗

---

### `ambient/filesystem-watcher.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `start(paths)` | `paths: string[]` | Watches dirs; auto-ingests new files |
| `stop()` | — | Stops watcher |

**Test:** ✗

---

### `actions/detector.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `detectActions(responseText)` | `responseText: string` | `ActionSuggestion[]` (top 3, heuristic scoring ≥2 keywords) |
| `enrichActionParams(responseText, actionType, userMessage)` | strings | `Promise<params object>` (LLM pre-fill) |

Action types: `email`, `task`, `save_drive`, `save_library`, `notion`.

**Test:** ✗

---

### `actions/executor.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `executeAction(execution)` | `execution: { id, action_type, params }` | `Promise<result object>` |

Routes by type: `save_library` → `engine.ingest()`, others → `toolRouter.callTool()`. Updates DB status: `executing` → `executed` or `failed`.

**Test:** ✗

---

### `briefing/generator.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `generate()` | — | `Promise<{ markdown, sections }>` |

**Test:** ✗

---

### `provenance/citation-mapper.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `map(responseText, contextSections)` | `responseText: string`, `contextSections: Section[]` | `CitationMap[]` |

**Test:** ✗

---

### `provenance/verifier.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `verify(sectionId)` | `sectionId: string` | `Promise<{ ok: boolean, diff?: string }>` |

Checks section content_hash against stored vault; detects tampering.

**Test:** ✗

---

### `reliability/index.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `score(documentId)` | `documentId: string` | `Promise<{ score: number, breakdown: object }>` |

**Test:** ✗

---

### `standing/index.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `create(libraryId, query, schedule)` | `libraryId: string`, `query: string`, `schedule: string` | `Promise<standingQuery object>` |
| `runQuery(queryId)` | `queryId: string` | `Promise<answer object>` |
| `tick()` | — | Runs all due standing queries |

**Test:** ✗

---

### `voice/transcriber.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `transcribe(audioBuffer, mimeType)` | `audioBuffer: Buffer`, `mimeType: string` | `Promise<string>` (transcript) |

Uses OpenAI Whisper or local nodejs-whisper.

**Test:** ✗

---

### `writing/context-lookup.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `lookupContext({ sessionId, triggerText, cursorContext, libraryScope })` | object | `Promise<WritingSuggestion[]>` (top 5 by relevance_score) |

Pulls from: people store (capitalized word entity hints), fact-retriever per library, engine.assembleContext. Persists to `writing_suggestions`.

**Test:** ✗

---

## Intelligence Layer (`/intelligence/`)

### `index.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `init()` | — | Sets up schema + subscribes to `document:complete` |

Triggers all extractors (entities, graph, timeline, decisions, people) sequentially per document.

**Test:** ✗

---

### `schema.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `applyIntelligenceSchema(db)` | `db: Database` | Creates intelligence tables |

**Test:** ✗

---

Each intelligence sub-module (`entities`, `decisions`, `graph`, `patterns`, `insights`, `cognitive`, `timeline`, `proactive`) follows this structure:

| File | Key Exports |
|------|-------------|
| `*/index.js` | `processDocument(documentId, libraryId)` → `Promise<void>` |
| `*/extractor.js` | `extract(content, documentId)` → `Promise<extracted items>` |
| `*/store.js` | `insert(items)`, `getByDocument(id)`, `getByLibrary(id)` |
| `graph/builder.js` | `build(entities, documentId)` → graph edges |
| `graph/traversal.js` | `traverse(entityId, depth?)` → connected nodes |

**Tests:** ✗ (all intelligence modules)

---

## People (`/people/`)

### `store.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `upsertPerson(personData)` | `{ canonical_name, aliases, bio, role, organization, email, linkedin }` | `string` (person ID) |
| `addInteraction(personId, data)` | `personId: string`, `data: object` | `void` |
| `addCommitment(personId, data)` | `personId: string`, `data: object` | `string` (commitment ID) |
| `fulfillCommitment(commitmentId)` | `commitmentId: string` | `void` (decrements `open_commitments_count`) |
| `addSignal(personId, data)` | `personId: string`, `data: object` | `void` |
| `getPerson(id)` | `id: string` | `person object \| null` |
| `listPeople(opts?)` | `{ search?, limit?, offset? }` | `Person[]` |
| `searchPeople(query)` | `query: string` | `Person[]` |
| `getInteractions(personId, limit?)` | `personId: string`, `limit?: number` | `Interaction[]` |
| `getCommitments(personId)` | `personId: string` | `Commitment[]` |
| `updateCardMd(personId, markdown)` | `personId: string`, `markdown: string` | `void` |
| `updateSentiment(personId, score, trend)` | `personId: string`, `score: number`, `trend: string` | `void` |
| `linkEntityToPerson(entityId, personId)` | strings | `void` |
| `getOverdueCommitments()` | — | `Commitment[]` (past due date, unfulfilled) |

**Test:** ✗

---

### `extractor.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `preScreenPersonMentions(text)` | `text: string` | `string[]` (person name candidates via `compromise`) |
| `extractPeopleFromDocument({ content_md, document_id, title })` | object | `Promise<ExtractedPerson[]>` (LLM; skips if <2 mentions) |

**Test:** ✗

---

### `resolver.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `findExistingPerson(canonicalName, aliases)` | `canonicalName: string`, `aliases: string[]` | `string \| null` (existing person ID; JaroWinkler ≥0.88) |
| `resolvePerson(personData)` | `personData: object` | `Promise<string>` (ID — existing or newly created) |

**Test:** ✗

---

### `sentiment-tracker.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `scoreSentiment(text)` | `text: string` | `number` (-1 to 1, AFINN normalized) |
| `recalculateSentiment(personId)` | `personId: string` | `Promise<{ score, trend }>` (avg of last 20; trend from comparing recent 5 vs older 5) |

**Test:** ✗

---

### `card-generator.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `generateCard(personId)` | `personId: string` | `Promise<string>` (3–5 sentence markdown card; saved to DB via `store.updateCardMd()`) |

**Test:** ✗

---

### `index.js` (people)
| Export | Inputs | Output |
|--------|--------|--------|
| `processDocument(documentId, libraryId)` | `documentId: string`, `libraryId: string` | `Promise<void>` |

Orchestrator: get doc → pre-screen → LLM extract → resolve → add interactions/commitments/signals → recalculate sentiment → deferred card generation (every 3 interactions or if no card yet).

**Test:** ✗

---

## MCP (`/mcp/`)

### `registry.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `register(connector)` | `connector: ConnectorInterface` | `void` |
| `get(id)` | `id: string` | `ConnectorInterface \| null` |
| `list()` | — | `ConnectorInterface[]` |
| `restore()` | — | `Promise<void>` (re-authenticates persisted connectors) |

**Test:** ✗

---

### `connector-interface.js`
| Export | — |
|--------|---|
| `ConnectorInterface` | Base class; all connectors extend this |

Methods: `connect(credentials)`, `disconnect()`, `listTools()`, `callTool(name, params)`, `getStatus()`.

**Test:** ✗

---

### `tool-router.js`
| Export | Inputs | Output |
|--------|--------|--------|
| `callTool(toolName, params, context?)` | `toolName: string`, `params: object`, `context?: object` | `Promise<result>` |
| `listAllTools()` | — | `Tool[]` (from all connected connectors) |

**Test:** ✗

---

### `db.js` (MCP)
| Export | Inputs | Output |
|--------|--------|--------|
| `saveCredentials(connectorId, credentials)` | `connectorId: string`, `credentials: object` | `void` (encrypted) |
| `loadCredentials(connectorId)` | `connectorId: string` | `object \| null` (decrypted) |
| `deleteCredentials(connectorId)` | `connectorId: string` | `void` |

**Test:** ✗

---

### Connectors (`/mcp/connectors/`) — 18 files

Each connector extends `ConnectorInterface`. Common shape:

| Method | Inputs | Output |
|--------|--------|--------|
| `connect(credentials)` | credentials object | `Promise<void>` |
| `listTools()` | — | `Tool[]` |
| `callTool(name, params)` | `name: string`, `params: object` | `Promise<result>` |

| Connector | Key Tools |
|-----------|-----------|
| `gmail.js` | search, send, read, label |
| `google-drive.js` | list, read, upload, create |
| `google-calendar.js` | list events, create, update |
| `github.js` | list repos, read file, create issue, PR |
| `notion.js` | search, get page, create page, update |
| `slack.js` | list channels, send message, read history |
| `apple-notes.js` | list, create, read, search |
| `apple-reminders.js` | list, create, complete |
| `linear.js` | list issues, create, update |
| `obsidian.js` | list, read, write, search |
| `readwise.js` | list highlights, get document |
| `spotify.js` | search, play, get current |
| `telegram.js` | send message, get updates |
| `youtube.js` | search, get transcript, get metadata |
| `plaid.js` | get accounts, get transactions |
| `ical.js` | fetch calendar, list events |
| `openai.js` | complete, embed |
| `anthropic.js` | complete, stream |

**Tests:** ✗ (all connectors)

---

## Operations (`/operations/`)

| File | Export | Inputs | Output |
|------|--------|--------|--------|
| `map.js` | `run(libraryId, prompt)` | `libraryId: string`, `prompt: string` | `Promise<MapResult>` |
| `distill.js` | `run(libraryId, focus?)` | `libraryId: string`, `focus?: string` | `Promise<DistillResult>` |
| `optimize.js` | `run(libraryId)` | `libraryId: string` | `Promise<OptimizeResult>` |
| `audit.js` | `run(libraryId)` | `libraryId: string` | `Promise<AuditResult>` |
| `export.js` | `run(libraryId, format)` | `libraryId: string`, `format: 'md'\|'pdf'\|'zip'` | `Promise<Buffer>` |

**Tests:** ✗ (all operations)

---

## Coverage Summary

| Area | Files | Tested |
|------|-------|--------|
| Middleware | 3 | 0 |
| Lib (core) | 9 | 0 |
| Collectors | 6 | 0 |
| Engine (core) | 13 | 1 (`intake.js` via smoke test) |
| Engine stores | 5 | 0 |
| Engine media | 5 | 0 |
| Routes | 21 | 0 |
| Features | 10 | 0 |
| Intelligence | ~26 | 0 |
| People | 7 | 0 |
| MCP | 23 | 0 |
| Operations | 5 | 0 |
| **Total** | **~133** | **1** |

> The in-memory DB helper (`tests/helpers/memDb.js`) is infrastructure, not a test itself.  
> Effective coverage: **<1%** of the codebase.

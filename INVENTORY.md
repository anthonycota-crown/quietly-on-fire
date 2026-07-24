# ContextOS — Full File Inventory
*Generated 2026-06-22. Excludes node_modules, dist build artifacts, binary assets, and lock files.*

---

## Summary

| Layer | Files |
|---|---|
| Client (React/Vite) | 94 |
| Server (Node/Express) | 197 |
| Desktop (Electron) | 2 |
| Mobile (React Native / Expo) | 14 |
| Config / root | 4 |
| **Total source files** | **311** |

**Test coverage: 44 suites · 786 tests · 786 passing**

---

## Root

```
contextos_build/
├── .env.example
├── CLAUDE.md
├── INVENTORY.md
└── package.json
```

---

## Client — `client/`

### Config
```
client/
├── index.html
├── netlify.toml
├── package.json
├── postcss.config.js
├── tailwind.config.js
└── vite.config.js
```

### Entry
```
client/src/
├── App.jsx                              # Router root, ToastProvider, ErrorBoundary wrapper
├── index.css                            # Global design tokens, animations, utility classes
└── main.jsx                             # Vite entry — mounts React app
```

### API
```
client/src/api/
└── client.js                            # Fetch wrapper — GET/POST/PUT/DELETE/postForm helpers
```

### Hooks
```
client/src/hooks/
├── useChat.js                           # Chat sessions, messages, streaming, send/cancel
├── useDocuments.js                      # Document list + CRUD for a library
├── useIntelligence.js                   # Intelligence layer — entities, graph, insights, cognitive map
├── useLibraries.js                      # Library list + CRUD
├── useMCP.js                            # MCP connector state — connect, disconnect, tool list
├── useMemories.js                       # Persistent memory facts — fetch, add, delete
├── useNotices.js                        # System notices + proactive intelligence notices
├── useOperations.js                     # Operation history and status polling
├── usePeople.js                         # People graph — list, profiles, sentiment
├── useRecents.js                        # Recently accessed libraries + documents
├── useSearch.js                         # Debounced full-text + semantic search
├── useSpotlights.js                     # Spotlight callouts — proactive insights surfaced in UI
├── useVizData.js                        # Visualization data fetch for knowledge map
├── useVoice.js                          # Voice input — recording, transcription state
└── useWriting.js                        # Writing assistant state — drafts, suggestions
```

### Pages
```
client/src/pages/
├── ActionsPage.jsx                      # Detected + scheduled actions list with run/dismiss controls
├── AmbientInbox.jsx                     # Ambient capture inbox — clipboard + filesystem captures
├── Collect.jsx                          # Top-level collector hub — picks a collector type
├── CollectType.jsx                      # Renders the chosen collector (URL / Paste / File / …)
├── Dashboard.jsx                        # Library grid + empty state + new-library flow
├── EntityPage.jsx                       # Entity detail — mentions, connections, timeline
├── Inbox.jsx                            # All-documents inbox with search + filter
├── Library.jsx                          # Per-library view — chat + document panel + SideRoom
├── PeoplePage.jsx                       # People directory — cards, sentiment, relationship graph
├── QuickCapture.jsx                     # Electron/popup quick-capture window
├── Search.jsx                           # Global semantic search with result cards
├── Settings.jsx                         # LLM provider config + storage backend config
└── WritingPage.jsx                      # Writing assistant — draft editor, context lookup, export
```

### Components — Layout
```
client/src/components/layout/
├── Breadcrumb.jsx                       # Page breadcrumb trail for deep navigation contexts
├── Layout.jsx                           # App shell — sidebar + <main> outlet + keyboard shortcuts
├── OperationStatusPill.jsx              # Floating pill showing active operation progress
└── Sidebar.jsx                          # Left nav — library list, global nav links, new-library button
```

### Components — Library
```
client/src/components/library/
├── IntelligencePanel.jsx                # Intelligence sidebar — entities, insights, decisions
├── LibraryCard.jsx                      # Card on the Dashboard — icon, name, doc count, color ring
├── LibraryChat.jsx                      # Chat pane — session sidebar + message list + input
├── LibraryDocumentPanel.jsx             # Document list pane inside a library view
├── MemoryDiffPanel.jsx                  # Diff view showing memory changes from a conversation
├── MemoryPanel.jsx                      # Persistent memory facts panel — view, add, delete
├── SinceLastVisit.jsx                   # Summary of changes since the user's last visit
├── StandingQueriesPanel.jsx             # Standing query list — create, view results, schedule
└── SynthesisPanel.jsx                   # On-demand synthesis results panel
```

### Components — Chat
```
client/src/components/chat/
├── ActionBar.jsx                        # Per-message action bar — copy, save, flywheel accept/dismiss
├── ChatInput.jsx                        # Textarea + send/stop button + streaming state
├── ChatMessage.jsx                      # Renders a single chat turn (user or assistant) with copy
├── StreamingMessage.jsx                 # Animated typing indicator during AI response streaming
└── VizCard.jsx                          # Inline visualization card rendered inside a chat message
```

### Components — SideRoom
```
client/src/components/sideroom/
├── AddZone.jsx                          # "Add" tab — collector picker inside the SideRoom panel
├── OperationHistory.jsx                 # Scrollable list of past operations with status badges
├── SideRoom.jsx                         # Slide-in panel — tab bar for Add / Tools, dialog semantics
└── ToolsZone.jsx                        # "Tools" tab — tool cards, running state, result viewer
```

### Components — SideRoom Tools
```
client/src/components/sideroom/tools/
├── AuditResult.jsx                      # Renders audit operation results — score ring, issue sections
├── DistillResult.jsx                    # Renders distill operation result — editable master brief
├── ExportPanel.jsx                      # Export flow — format picker, options, filename, download
├── MapResult.jsx                        # D3 force-graph map of document topics/entities
└── OptimizeResult.jsx                   # Renders optimize results — per-doc recommendations
```

### Components — Collectors
```
client/src/components/collectors/
├── CodeCollector.jsx                    # Code snippet drop + syntax type selector
├── EmailCollector.jsx                   # Raw email paste collector
├── FileCollector.jsx                    # Drag-and-drop file upload collector
├── PasteCollector.jsx                   # Plain text / rich text paste collector
├── UrlCollector.jsx                     # URL ingest + shared SuccessCard / ErrorBar / IngestButton
└── YoutubeCollector.jsx                 # YouTube URL collector + oembed preview
```

### Components — Documents
```
client/src/components/documents/
├── DocumentCard.jsx                     # Compact doc card used in Inbox + search results
├── DocumentPreview.jsx                  # Full document read-only preview panel
└── DocumentTagEditor.jsx                # Inline tag pill editor — add / remove tags
```

### Components — Viz
```
client/src/components/viz/
└── VizGallery.jsx                       # Gallery of extracted visualizations from documents
```

### Components — UI primitives
```
client/src/components/ui/
├── Badge.jsx                            # Status badge (complete / running / error / pending)
├── BulkActionBar.jsx                    # Multi-select action bar — bulk tag, delete, move
├── Button.jsx                           # Base button — variant (primary/outline/ghost), size, icon
├── CommandPalette.jsx                   # Keyboard-driven command palette (⌘K)
├── EmptyState.jsx                       # Empty state illustration + CTA for zero-data screens
├── ErrorBoundary.jsx                    # React error boundary — catches render errors, retry CTA
├── FilterChips.jsx                      # Tag/filter chip row with add and remove
├── InfoTooltip.jsx                      # ⓘ icon with hover tooltip for contextual help
├── MeetingBriefPanel.jsx                # Pre-meeting brief panel from calendar + library context
├── Modal.jsx                            # General purpose modal — dialog, focus trap, Escape close
├── NextActionCard.jsx                   # Proactive next-action suggestion card
├── NotificationBell.jsx                 # Bell icon + unread badge + notification dropdown
├── OnboardingChecklist.jsx              # First-run checklist — tracks setup steps
├── ShortcutSheet.jsx                    # Keyboard shortcut reference sheet (modal)
├── Skeleton.jsx                         # Skeleton loading placeholder shapes
├── Spinner.jsx                          # Animated loading spinner
├── SpotlightCallout.jsx                 # Highlighted proactive insight callout in the UI
├── Toast.jsx                            # Toast notification system — provider + useToast hook
└── VoiceModal.jsx                       # Voice input modal — waveform, recording state, transcript
```

---

## Server — `server/`

### Entry
```
server/
├── index.js                             # Express app bootstrap — mounts all routes, middleware, scheduler,
│                                        # intelligence.init(), mcp.restoreConnections(), ambient, reclaimOrphans()
└── package.json
```

### Routes
```
server/routes/
├── actions.js                           # GET/POST /actions — detected + user-created actions
├── ambient.js                           # GET/POST /ambient — clipboard + filesystem captures
├── briefing.js                          # GET /briefing — pre-meeting brief generation
├── chat.js                              # POST /chat — streaming SSE chat; calls flywheel.scoreTurn post-turn
├── collect.js                           # POST /collect/:type — URL, paste, file, email, YouTube, code
├── documents.js                         # GET/PUT/DELETE /documents — document CRUD + tag updates
├── export.js                            # POST /export — PDF, DOCX, Markdown zip, JSON export
├── flywheel.js                          # GET /flywheel/messages/:id/candidate; POST …/accept|dismiss;
│                                        # GET /flywheel/compost; GET /flywheel/compost/patterns
├── intelligence.js                      # GET /intelligence — entities, graph, insights, cognitive map
├── libraries.js                         # GET/POST/PUT/DELETE /libraries — library CRUD
├── mcp.js                               # GET/POST /mcp — connector registry, connect, tool invoke
├── me.js                                # GET /me — user profile + behavioral summary
├── memory.js                            # GET/POST/DELETE /memory — persistent memory facts
├── notices.js                           # GET /notices — proactive intelligence notices
├── notifications.js                     # GET /notifications — SSE push notification stream
├── operations.js                        # GET/POST /operations — map/audit/optimize/distill/export jobs
├── output.js                            # GET /output — structured output documents
├── people.js                            # GET /people — person cards, sentiment, relationship graph
├── search.js                            # GET /search — semantic + full-text search
├── settings.js                          # GET/PUT /settings — LLM config, storage backend config
├── standing.js                          # GET/POST /standing — standing query CRUD + tick results
├── synthesis.js                         # POST /synthesis — on-demand synthesis endpoints
├── system.js                            # GET /system — health check, version info, auth verify
├── voice.js                             # POST /voice — audio transcription via Whisper
└── writing.js                           # POST /writing — writing assistant context + draft generation
```

### Behavioral Intelligence — Wave 0 + Wave 1
```
server/behavioral/
├── baseline.js                          # Per-metric baseline — upsert, fetch, rolling average
├── capture.js                           # captureEvent() — inserts behavioral_events, triggers analysis
├── dictionaries.js                      # Word-list dictionaries: absolutist, hedge, agency (active/passive)
├── flags.js                             # Flag emission — z-score threshold check → behavioral_flags row
├── index.js                             # Public API — runMetrics(), captureEvent(), getFlags()
├── registry.js                          # Metric registry — ordered list of all active metric modules
├── schema.js                            # addIfMissing migrations for behavioral tables + seed config rows
└── tokenizer.js                         # Word tokenizer + LIWC-style category counter
```

```
server/behavioral/metrics/
├── absolutist-density.js                # absolutist_count/word_count × 1000 over 7d (tier1)
├── agency-score.js                      # (active−passive)/(active+passive) over 7d, range [−1,+1] (tier1)
├── hedge-density.js                     # hedge_count/word_count × 1000 over 7d (tier1)
├── pronoun-ratio.js                     # i_total/(i_total+we_total) over 7d (tier1)
├── recurrence-with-resolution.js        # Fraction of library-days that are return visits (≤6d gap) over 30d (tier2)
├── time-of-day-clustering.js            # Fraction of 30d events in peak 6h sliding window (tier2)
└── topic-obsession.js                   # Top library's share of events over 7d (tier1, Wave 0)
```

### Flywheel
```
server/flywheel/
├── compost.js                           # writeCompost() inserts flywheel_compost row (weight=0.1);
│                                        # compostPatterns() GROUP BY theme_key HAVING count≥N
├── detector.js                          # scoreFromSignals() — pure arithmetic 0→1; exports
│                                        # CANDIDATE_THRESHOLD=0.55, COMPOST_THRESHOLD=0.25
├── index.js                             # scoreTurn() — signals→score→UPDATE chat_messages→writeCompost;
│                                        # re-exports precipitate, compostPatterns
├── precipitate.js                       # async precipitate() — distill.runOnText→documents.insert→
│                                        # vault-writer.write→ledger update→stamp message accepted
├── schema.js                            # addIfMissing: flywheel_score/signals/state/output_doc_id on
│                                        # chat_messages; CREATE flywheel_compost + 2 indexes
└── signals.js                           # extractSignals() — answerWords, groundedClaims, distinctSources,
│                                        # librariesTouched, noveltyScore from retrieval/grounding/provenance
```

### Collectors
```
server/collectors/
├── code.js                              # Code snippet ingest — language detection, chunking
├── email.js                             # Email parse + header extraction
├── file.js                              # Multer file upload handler — PDF, DOCX, XLSX, TXT, MD, image
├── paste.js                             # Plain text ingest
├── url.js                               # URL fetch + readability extraction + SSRF guard
└── youtube.js                           # YouTube transcript fetch via oembed + subtitles API
```

### Operations (batch AI jobs)
```
server/operations/
├── audit.js                             # Library audit — staleness, gaps, broken links scoring
├── distill.js                           # runDistill() batch brief + runOnText() single-turn distill
│                                        # for flywheel precipitate path
├── export.js                            # Export — PDF, DOCX, Markdown zip, TXT, JSON
├── map.js                               # Knowledge map — entity/topic graph data for D3
└── optimize.js                          # Optimization — per-doc LLM recommendations
```

### Engine (RAG pipeline)
```
server/engine/
├── context-assembler.js                 # Builds context window for a chat query
├── conversation-compressor.js           # Compresses old message history to stay within token budget
├── dataset-extractor.js                 # Extracts structured datasets (tables, lists) from documents
├── duplicate-detector.js                # Near-duplicate detection via SHA-256 before ingestion
├── enrichment.js                        # Enrichment pass — keywords, summary, metadata extraction
├── errors.js                            # Engine-specific error types
├── events.js                            # EventEmitter bus — document:complete|deleted|deleting
├── fact-retriever.js                    # Retrieves atomic facts relevant to a query
├── fast-path.js                         # Heuristic fast-path bypass for simple queries using FTS5
├── grounding-checker.js                 # Post-generation grounding / hallucination check
├── index.js                             # Engine orchestrator — ingest, assembleContext, groundResponse, reclaimOrphans
├── intake.js                            # Document intake pipeline entry point
├── parser.js                            # Text parser — chunking, section detection
├── query-rewriter.js                    # Rewrites user query for better retrieval
├── section-retriever.js                 # Retrieves relevant document sections
├── synthesis.js                         # LLM synthesis layer — generates final answer
└── vault-writer.js                      # Writes processed docs to storage backend
```

```
server/engine/media/
├── code.js                              # Code-specific processing (AST hints, language metadata)
├── index.js                             # Media handler dispatcher
├── structured.js                        # Structured data handler (CSV, XLSX, JSON)
├── temporal.js                          # Temporal extraction (dates, events, timelines)
└── visual.js                            # Visual media handler (image OCR / description)
```

```
server/engine/stores/
├── chunks.js                            # Vector chunk storage — embed + upsert + similarity search
├── facts.js                             # Atomic fact store — write + retrieval
├── registry.js                          # Document registry — metadata index
├── sections.js                          # Section store — headings, passages
└── summaries.js                         # Summary store — per-doc and per-library summaries
```

### Features
```
server/features/actions/
├── detector.js                          # Detects action items from conversation and documents
└── executor.js                          # Executes scheduled or user-triggered actions
```

```
server/features/ambient/
├── clipboard-watcher.js                 # macOS clipboard polling — detects new text/URL copies
├── filesystem-watcher.js                # chokidar watcher for configured ingest directories
└── index.js                             # Ambient feature coordinator — start/stop watchers
```

```
server/features/briefing/
└── generator.js                         # Pre-meeting brief generator — pulls calendar + library context
```

```
server/features/crosslibrary/
└── signal-detector.js                   # Detects signals that span multiple libraries
```

```
server/features/export/
└── vault-export.js                      # Vault-format export — writes docs to Obsidian-compatible structure
```

```
server/features/me-profile/
└── index.js                             # Me profile — aggregates behavioral + library stats for /me
```

```
server/features/memory/
├── extractor.js                         # Extracts persistent memory facts from conversation turns
└── index.js                             # Memory feature coordinator — extract + store
```

```
server/features/memorydiff/
└── index.js                             # Computes memory diff between two conversation snapshots
```

```
server/features/output/
└── index.js                             # Output document store — structured AI-generated outputs
```

```
server/features/provenance/
├── citation-mapper.js                   # Maps answer spans back to source document chunks
├── index.js                             # Provenance feature coordinator
└── verifier.js                          # Verifies cited claims against source text
```

```
server/features/reliability/
└── index.js                             # Source reliability scoring — rescores on document completion
```

```
server/features/standing/
└── index.js                             # Standing queries — store, tick on cadence, return results
```

```
server/features/voice/
└── transcriber.js                       # Audio transcription via Whisper API
```

```
server/features/writing/
└── context-lookup.js                    # Fetches library context to ground writing assistant drafts
```

### Intelligence (background enrichment)
```
server/intelligence/
├── index.js                             # Orchestrator — init, subscribe to document:complete events
├── schema.js                            # Shared DB schema definitions for intelligence tables
└── util.js                              # Shared helpers (LLM calls, normalisation)
```

```
server/intelligence/cognitive/
├── generator.js                         # LLM-based cognitive map generation
├── index.js                             # Cognitive map coordinator
└── store.js                             # Cognitive map persistence
```

```
server/intelligence/decisions/
├── extractor.js                         # LLM extractor for decisions and action items
├── index.js                             # Decision extraction coordinator
└── store.js                             # Decision storage + retrieval
```

```
server/intelligence/entities/
├── extractor.js                         # NER — people, orgs, places, concepts
├── index.js                             # Entity pipeline coordinator
├── normalizer.js                        # Entity deduplication + canonical name resolution
└── store.js                             # Entity storage + lookup
```

```
server/intelligence/graph/
├── builder.js                           # Builds edges between entities + documents
├── extractor.js                         # LLM-based relationship extraction
├── index.js                             # Knowledge graph coordinator
├── store.js                             # Graph edge/node persistence
└── traversal.js                         # Graph traversal helpers (BFS/DFS, path finding)
```

```
server/intelligence/insights/
├── generator.js                         # LLM-generated insight bullets from document clusters
├── index.js                             # Insights coordinator
└── store.js                             # Insight storage
```

```
server/intelligence/patterns/
├── detector.js                          # Cross-document pattern / theme detection
├── index.js                             # Pattern detection coordinator
└── store.js                             # Pattern storage
```

```
server/intelligence/proactive/
├── index.js                             # Proactive intelligence coordinator
├── jobs.js                              # Job definitions — what to run proactively
├── scheduler.js                         # Cron-style scheduler for proactive jobs
└── store.js                             # createNotice() + job run history storage
```

```
server/intelligence/recommendations/
├── generator.js                         # LLM-based recommendations for library growth
├── index.js                             # Recommendations coordinator
└── store.js                             # Recommendation storage
```

```
server/intelligence/timeline/
├── extractor.js                         # Extracts dated events and sequences from documents
├── index.js                             # Timeline extraction coordinator
└── store.js                             # Timeline event storage
```

### MCP Connectors
```
server/mcp/
├── calendar-source.js                   # iCal calendar source adapter for the event aggregator
├── connector-interface.js               # Base interface / contract for all MCP connectors
├── index.js                             # MCP event aggregator — register, poll, dispatch
├── registry.js                          # Connector registry — connect, disconnect, restoreConnections()
└── tool-router.js                       # Routes tool-invoke requests to the correct connector
```

```
server/mcp/connectors/
├── anthropic.js                         # Anthropic MCP connector (Claude tool use bridge)
├── apple-notes.js                       # Apple Notes connector via AppleScript
├── apple-reminders.js                   # Apple Reminders connector via AppleScript
├── gmail.js                             # Gmail connector via Google OAuth + Gmail API
├── google-calendar.js                   # Google Calendar connector
├── google-drive.js                      # Google Drive connector — read docs, search
├── ical.js                              # Generic iCal/CalDAV connector
├── linear.js                            # Linear issue tracker connector
├── notion.js                            # Notion connector — read pages + databases
├── obsidian.js                          # Obsidian vault connector — read/write .md files
├── openai.js                            # OpenAI connector (tool use bridge)
├── plaid.js                             # Plaid financial data connector
├── readwise.js                          # Readwise connector — highlights + articles
├── slack.js                             # Slack connector — channels, messages, search
├── spotify.js                           # Spotify connector — recently played, playlists
├── telegram.js                          # Telegram connector — saved messages
└── youtube.js                           # YouTube connector — subscriptions, watch history
```

### Lib (shared server utilities)
```
server/lib/
├── config.js                            # Runtime config — reads env vars, sets defaults, reads data/config.json
├── embeddings.js                        # Embedding generation — calls LLM or local embedder
├── ingest-bus.js                        # EventEmitter for ingest pipeline progress events
├── library-bus.js                       # EventEmitter for library-level events
├── llm.js                               # LLM client — complete/stream/vision; wraps Anthropic SDK
├── migrations.js                        # Migration runner — executes numbered migration files in order
├── push.js                              # SSE push helper — VAPID init + sends events to connected clients
├── retrieval.js                         # Shared retrieval utilities (TF-IDF) used across engine + routes
├── scheduler.js                         # Global job scheduler — node-cron, behavioral metrics tick hourly,
│                                        # flywheel compost sweep daily at 09:00 UTC
└── storage.js                           # Storage abstraction — delegates to local / Obsidian backends
```

```
server/lib/migrations/
├── 0001_core.js                         # Core schema — libraries, documents, chat, operations
├── 0002_intelligence.js                 # Intelligence tables — entities, graph, insights, timeline
├── 0003_mcp.js                          # MCP tables — connector registry, connector sessions
├── 0004_datasets_status.js              # Datasets table + document status column
├── 0005_chat_session_mode.js            # mode column on chat_sessions
├── 0006_me_profile.js                   # me_profile table
├── 0007_memory_layer.js                 # memory_facts + memory_events tables
├── 0008_output_actions.js               # output_documents + actions tables
├── 0009_recommendations.js              # recommendations table
└── index.js                             # Exports ordered migration list
```

```
server/lib/storage/
├── local.js                             # Local filesystem storage backend
└── obsidian.js                          # Obsidian vault storage backend — reads/writes .md files
```

### Middleware
```
server/middleware/
├── auth.js                              # Auth middleware — bearer token, constant-time compare, localhost bypass
├── rateLimit.js                         # express-rate-limit — apiLimiter (300/min), strictLimiter (60/min)
└── validate.js                          # Request body validation helpers + Zod schemas
```

### People
```
server/people/
├── card-generator.js                    # Generates rich person cards from extracted data
├── extractor.js                         # Extracts person mentions and attributes from text
├── index.js                             # People feature coordinator — processDocument
├── resolver.js                          # Deduplicates and resolves person entities across documents
├── sentiment-tracker.js                 # Tracks sentiment toward specific people over time
└── store.js                             # Person storage — upsert, fetch, relationship edges
```

### Tests
```
server/tests/helpers/
└── memDb.js                             # In-memory SQLite — runs full runMigrations() for test isolation;
│                                        # exports db, resetDb()
```

```
server/tests/
├── auth.test.js                         # Auth middleware — valid key, missing key, rate-limit bypass
├── behavioral.wave0.test.js             # Wave 0: captureEvent, topic_obsession metric, flag emission (z-score)
├── behavioral.wave1.test.js             # Wave 1: 12 tests — pronoun-ratio, absolutist-density, agency-score,
│                                        #   hedge-density, time-of-day-clustering, recurrence-with-resolution,
│                                        #   runMetrics smoke
├── config.test.js                       # Config load/get/set/reset with defaults
├── db-schema.test.js                    # Schema migrations — tables + columns present
├── errors.test.js                       # Engine error type hierarchy
├── flywheel.test.js                     # 9 flywheel assertions — schema, detector (3 verdicts), scoreTurn,
│                                        #   compost write, precipitate success + failure, compostPatterns
├── hybrid.pipeline.test.js              # Hybrid retrieval pipeline integration
├── intake.smoke.test.js                 # End-to-end ingest pipeline smoke test
├── integration.test.js                  # Multi-route integration test
├── intel-jobs.test.js                   # Proactive intelligence job scheduler
├── mcp-db.test.js                       # MCP DB schema + credential encrypt/decrypt
├── rateLimit.test.js                    # Rate limiter — sliding window, strict tier
├── search-route.test.js                 # Search route — semantic + full-text query handling
├── stress.perf.test.js                  # Stress / performance — concurrent ingest throughput
├── t1.auth.test.js                      # Tier-1: auth edge cases
├── t1.collect-route-field-names.test.js # Tier-1: collector route field validation
├── t1.db-schema.test.js                 # Tier-1: schema parity — all expected tables + columns present
├── t1.export-vault-path-traversal.test.js # Tier-1: path traversal guard in vault export
├── t1.mcp-db.test.js                    # Tier-1: MCP DB schema
├── t1.rate-limit.test.js                # Tier-1: rate-limit config values
├── t1.url-collector.test.js             # Tier-1: URL collector SSRF guard
├── t1.validate.test.js                  # Tier-1: request validation middleware
├── t2.buses.test.js                     # Tier-2: ingest-bus + library-bus event emission
├── t2.code-collector.test.js            # Tier-2: code collector language detection
├── t2.email-collector.test.js           # Tier-2: email parse + header extraction
├── t2.file-collector.test.js            # Tier-2: file upload handler
├── t2.paste-collector.test.js           # Tier-2: paste collector
├── t2.people-extractor.test.js          # Tier-2: person mention extraction
├── t2.resolver.test.js                  # Tier-2: person entity deduplication
├── t2.sentiment-tracker.test.js         # Tier-2: sentiment tracking over time
├── t2.youtube-collector.test.js         # Tier-2: YouTube transcript collector
├── t3.duplicate-detector.test.js        # Tier-3: near-duplicate detection
├── t3.embeddings.test.js                # Tier-3: embedding generation + similarity
├── t3.fact-retriever.test.js            # Tier-3: atomic fact retrieval
├── t3.llm.test.js                       # Tier-3: LLM client streaming + error handling
├── t3.parser.test.js                    # Tier-3: text parser chunking + section detection
├── t3.retrieval.test.js                 # Tier-3: hybrid retrieval pipeline
├── t3.section-retriever.test.js         # Tier-3: section retriever scoring
├── t3.validate-schema-parity.test.js    # Tier-3: validates DB schema against migration list
├── t3.viz-extract.test.js               # Tier-3: visualization extraction from documents
├── url-redirect-ssrf.test.js            # SSRF guard — redirect following blocked
├── url-ssrf.test.js                     # SSRF guard — private IP ranges blocked
└── validate.test.js                     # Validation helper unit tests
```

---

## Desktop — `desktop/`

```
desktop/
└── package.json
```

```
desktop/src/
├── main.js                              # Electron main process — window management, tray, IPC
└── preload.js                           # Electron preload — exposes safe electronAPI to renderer
```

---

## Mobile — `mobile/`

### Config
```
mobile/
├── app.json                             # Expo app config — name, slug, icon, splash, permissions
├── babel.config.js                      # Babel config for Expo
├── package.json
└── tsconfig.json
```

### App (Expo Router file-based routing)
```
mobile/app/
├── _layout.tsx                          # Root layout — navigation container, theme provider
└── capture.tsx                          # Quick-capture screen — paste URL or text into a library
```

```
mobile/app/(tabs)/
├── _layout.tsx                          # Tab bar layout — Home / Inbox / Search / Settings tabs
├── index.tsx                            # Home tab — library grid
├── inbox.tsx                            # Inbox tab — recent documents across all libraries
├── search.tsx                           # Search tab — semantic search
└── settings.tsx                         # Settings tab — API key, server URL config
```

```
mobile/app/library/
└── [id].tsx                             # Dynamic library screen — chat interface for a library
```

### API
```
mobile/src/api/
└── client.ts                            # Typed fetch wrapper — connects to ContextOS server
```

### Components
```
mobile/src/components/
├── ChatBubble.tsx                       # Chat message bubble (user / assistant)
├── DocumentCard.tsx                     # Document card for library and inbox lists
└── LibraryCard.tsx                      # Library card for the home grid
```

### Hooks
```
mobile/src/hooks/
├── useChat.ts                           # Chat state, send message, streaming
└── useLibraries.ts                      # Library list fetch + refresh
```

### Theme
```
mobile/src/
└── theme.ts                             # Design tokens — colors, spacing, typography for React Native
```

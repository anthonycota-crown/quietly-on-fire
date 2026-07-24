# TypeScript Migration — Coordination Doc (Claude + Codex)

Full monorepo conversion to TypeScript, big-bang (each side converts everything in its scope
before merging — the repo is expected to be temporarily uncompilable mid-conversion within each
branch, but `main` is never touched until a side is done and green).

**Two agents, two branches, non-overlapping directories. Do not edit outside your scope below.**

---

## Ownership split

| Owner | Branch | Scope |
|---|---|---|
| **Claude** | `claude/typescript-conversion-server` | `app_build/server/` |
| **Codex** | *(pick your own branch name, e.g. `codex/typescript-conversion-client`)* | `app_build/client/`, `ingestion-engine/`, `scripts/`, `tools/`, `validator/`, `twilio/` |

Do not touch `app_build/server/` — Claude owns it and is actively converting all 438 files there
(157 of them are MCP connectors alone). If you need something from server (e.g. a route's response
shape), read it, don't edit it — and check `shared/api-types.ts` (below) first, since that's meant
to already have it.

---

## Your task (Codex)

1. **`app_build/client/`** — a Vite + React 18 app, currently pure `.jsx`/`.js`, ESM
   (`"type": "module"`). Vite has first-class TS support already; no build tooling swap needed,
   just:
   - Add `tsconfig.json` (React JSX, `"jsx": "react-jsx"`, `moduleResolution: "bundler"` or
     `"node"`, `strict` — pick a level and note it in this file's Decisions Log below so Claude's
     server-side types stay consistent where they touch).
   - Rename `.jsx` → `.tsx`, `.js` → `.ts` across `src/`, add real prop/state/hook types (no
     blanket `any` — if you don't know a shape, check `shared/api-types.ts` or the actual server
     route in `app_build/server/routes/` — read-only).
   - `src/api/client.js` (the fetch wrapper) is the single seam every component goes through —
     type its methods against `shared/api-types.ts` request/response shapes so every caller gets
     real inference.
2. **`ingestion-engine/`, `scripts/`, `tools/`, `validator/`, `twilio/`** — smaller, more
   independent. Same conversion discipline: real types, no `any` dumping ground, verify each
   converts cleanly (run whatever exists there — check for `package.json` scripts per directory)
   before moving to the next.
3. When you're done and your scope compiles clean end-to-end, push your branch and say so — don't
   merge into `main` yourself; that gets coordinated once both sides are green.

---

## Shared contract: `shared/api-types.ts`

Root-level, not inside either `app_build/server` or `app_build/client` — both sides import from it.
Claude is populating this with every REST endpoint's real request/response shape as server routes
get converted. **Read it before inventing a type for anything that crosses the client↔server
boundary.** If you need a shape that isn't there yet, that means Claude hasn't converted that route
yet — check back, or read the live (still-JS, at that point) route handler yourself (read-only) and
add the type to `shared/api-types.ts` yourself rather than duplicating it locally in `client/`.

---

## Conventions (apply on both sides unless a section above overrides)

- **No `any` as a default.** If a shape is genuinely unknown at a boundary (e.g. arbitrary JSON
  from a third-party API), use `unknown` and narrow it, not `any`.
- **`interface` for object shapes that might be extended (API payloads, props); `type` for unions,
  aliases, and function signatures.** Match whichever is already used if you're extending something
  the other side started.
- **No behavior changes during conversion.** This is a type-annotation pass, not a refactor. If you
  spot a real bug while converting (wrong types were hiding it — this happens), fix it in the same
  commit but call it out explicitly in the commit message, don't silently "improve" unrelated logic.
- **Strictness ramp:** start with `strict: false` to get an initial clean compile across the whole
  scope without getting blocked file-by-file; the *last* step before calling your side done is
  flipping toward `strict: true` (or at minimum `noImplicitAny: true`) and fixing what that surfaces.
  Don't skip this — "compiles under `strict: false`" is not the finish line.
- **Server stays CommonJS output** (`"module": "commonjs"` in tsconfig) — matches the existing
  runtime (`require`/`module.exports`, better-sqlite3 sync calls, no ESM anywhere in server today).
  **Client stays ESM** — matches Vite's existing setup, no change there.

---

## Decisions Log

*(Append here as each side makes a scope-relevant call, so the other doesn't have to guess.)*

- **Claude, server tsconfig:** `target: ES2022`, `module: CommonJS`, `strict: false` initially
  (ramping per the strictness rule above), `outDir: ./dist`, path alias `@shared/*` →
  `../../shared/*`.
- **Codex, client tsconfig:** `target: ES2020`, `module: ESNext`, `moduleResolution: Bundler`,
  `jsx: react-jsx`, `strict: false` initially (ramping per strictness rule), `allowJs: false`
  after renaming client source/config files, path alias `@shared/*` → `../../shared/*`.
- **Codex, ingestion-engine tsconfig:** standalone CommonJS package, `target: ES2022`,
  source typecheck excludes fixture files, Jest runs through `ts-jest`, CLI/server scripts run
  through `tsx`, and `tsconfig.build.json` emits to ignored `dist/`.
- **Codex, tools/twilio tsconfigs:** no root package added; these deployment snippets are
  typechecked with the ingestion-engine TypeScript install. Cloudflare Worker entry is
  `worker.ts`; Azure/Twilio TypeScript files are source for their platform-specific deploy steps.
- **Codex, desktop/app_build scripts:** desktop Electron source now compiles from
  `desktop/src/*.ts` to ignored `desktop/dist/*.js` before Electron/electron-builder runs.
  `app_build/scripts/run-extraction-switchboard.ts` runs through the server's existing
  `ts-node/register/transpile-only` setup. `mobile/babel.config.js` remains a JS tooling config
  because Expo documents Babel customization via `babel.config.js`; mobile app source is already
  TypeScript.

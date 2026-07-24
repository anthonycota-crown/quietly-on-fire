# TASK_QUEUE — Single-Page Slide-In Workspace

Ordered, small, independently-verifiable. Status: TODO / WIP / DONE / BLOCKED.
Most of Phases A/B are already built (Docs 2–4B). Remaining work below.

## Phase A — prove & harden the shell
- **A0** Phase 0 discovery + baseline + logs — **DONE**
- **A1** Fix rail `navigate()` violations: `IntelligencePanel.openDoc` → open doc as evidence via state (`onOpenDocument`); keep entity-fallback navigate only for standalone use — **WIP**
- **A2** Search → workspace slide-in rail panel (reuse Search logic; scope to open library; chat persists) — TODO
- **A3** Add/Collect → workspace slide-in panel (reuse collectors; SideRoom already has an add zone — confirm/extend) — TODO
- **A4** Settings → workspace slide-in panel; fix `MeetingBriefPanel` → /settings to open the panel via state — TODO
- **A5** Writing → workspace slide-in panel (server-backed, prosemirror present) — TODO
- **A6** Three-state pass (loading/empty/error) on every new panel — TODO

## Phase B — structural inversion (objects first-class)
- **B1** ObjectDetailPanel on all 7 types — **DONE (Docs 4/4B)** — verify still green
- **B2** Documents demoted to evidence — **DONE (Doc 4B)**

## Phase C — remaining designed surfaces
- **C1** People panel — **DONE (Doc 4B)**
- **C2** Viz gallery panel + inline VizCard + brace-balanced `[VIZ]` parsing — **DONE (Docs 1/3)** — verify
- **C3** Trust & continuity: provenance chips + confidence badge + receipts **DONE (Doc 3)**; reliability-score tooltip on docs/evidence — verify/add; standing queries **DONE**; memory diff **DONE**
- **C4** Writing mode (covered by A5)
- **C5** Guide entry — **DONE (Guide MVP)** — confirm reachable
- **C6** Global chrome: notices/bell, meeting brief, voice, quick-capture — **mostly DONE** — fix brief→settings nav

## Phase D — UX optimization (tokens permitting)
- teaching empty states · trust tooltips · undo-toast · command-palette panel verbs · operation-status pill · SSE retry · recently-viewed · shortcut sheet · since-last-visit · next-best-action (rule-based)

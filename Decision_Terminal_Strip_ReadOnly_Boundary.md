# Terminal Strip Source-of-Truth / Read-only Boundary

## Scope

This note records current code evidence for terminal-strip ownership before any MAM-specific terminal-strip projection or regression fixture is added. It does not approve product code, UI, persistence, XML schema, migration, packaging, or runtime changes.

## Current Sources

- `QETProject` owns terminal strips as project-level objects. `QETProject::toXml()` serializes them below `<terminal_strips>`, and project load recreates `TerminalStrip` instances from that same project-level section.
- `TerminalStrip` owns strip metadata (`TerminalStripData`), ordered `PhysicalTerminal` entries, `RealTerminal` membership, and `TerminalStripBridge` objects.
- `RealTerminal` is a wrapper around a placed terminal `Element`; its persisted identity is the placed terminal element UUID via the `element_uuid` XML attribute.
- `TerminalStrip::fromXml()` resolves saved `element_uuid` values through `ElementProvider::freeTerminal()`. Terminal-strip membership therefore depends on existing placed terminal elements and does not create independent electrical terminals.
- `TerminalStripBridge` persists bridge UUID/color plus real-terminal element UUID references, then resolves them back through the strip's loaded `RealTerminal` objects.
- Terminal-strip graphics are separate diagram items via `TerminalStripItemXml`; those items reference and display a strip, but they are not the ownership source for strip membership.

## Boundary

For the current MAM fork phase, terminal-strip facts may only be consumed as read-only projection or regression evidence:

- Use existing `QETProject`, `TerminalStrip`, `PhysicalTerminal`, `RealTerminal`, and `TerminalStripBridge` state as observed input.
- Treat placed terminal `Element` UUIDs as the compatibility key for strip membership.
- Keep terminal-strip bridge evidence separate from conductor potential semantics unless a later slice proves and tests an explicit relationship.
- Do not infer a standalone cable/core/terminal ownership model from terminal strips alone.

Out of scope for this phase:

- XML schema changes or migration of `<terminal_strips>`.
- UI/editor behavior changes.
- Persistence repair, automatic strip generation, or mutation during projection.
- Device/Core or cable model ownership changes.
- Runtime, packaging, or installation validation.

## Test Implication

A small synthetic terminal-strip regression fixture is only acceptable if it is derived directly from the documented XML/code structure above and verifies load/resave compatibility of the existing project-level terminal-strip XML. It must not claim electrical-potential behavior, cable semantics, UI rendering, editor interaction, or migration behavior.

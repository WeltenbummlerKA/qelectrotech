# Progress

## 2026-09-21 - CAE Reference Analysis Phase 1

Completed:
- Created `CAE_Reference_Matrix.md` as a standalone, non-implementing reference matrix.
- Compared QET current findings against public vendor evidence for EPLAN Electric P8, WSCAD ELECTRIX, Zuken E3.series, AUCOTEC Engineering Base, SEE Electrical, and AmpereSoft ProPlan / Moeller-ProPlan only where publicly supportable.
- Covered workflows: contact mirrors/cross-references, device/function/symbol/article separation, terminals/cables/potentials, project/page structure, numbering/autonum, reports, data model/project DB, and import/export compatibility.

Evidence:
- QET evidence from `Brain.md`, `Progress.md`, and earlier code traces of `sources/qetproject.*`, `sources/diagram.*`, `sources/qetgraphicsitem/*`, `sources/TerminalStrip/*`, `sources/autoNum/*`, `sources/dataBase/projectdatabase.*`, `sources/cli_export.cpp`, and export helpers.
- Public vendor sources only: EPLAN product page, WSCAD product/release pages, Zuken E3.series product page, AUCOTEC Engineering Base product page, SEE Electrical public product table, AmpereSoft ProPlan product page.

Open risks:
- Public vendor pages describe principles and capabilities, not internal data schemas.
- Proprietary native formats, report template languages, device portals, and commercial catalog licensing remain unverified and must not be copied or assumed.
- ProPlan/Moeller lineage is only treated through current public AmpereSoft ProPlan evidence; historical Eaton/Moeller internals remain unverified.

Next recommended task:
- Perform a narrow decision-prep analysis for one ownership question before architecture work: either contact/cross-reference ownership or cable/core/potential ownership, using `CAE_Reference_Matrix.md` plus QET code evidence.

## 2026-09-21 - Contact/CrossRef Ownership Decision Analysis

Completed:
- Created `Decision_Contact_CrossRef_Ownership.md` as a non-implementing decision-prep document.
- Re-read QET contact/cross-reference code evidence for `ElementData`, `Element`, `MasterElement`, `ReportElement`, `LinkElementCommand`, `ContactUsage`, `CrossRefItem`, XML link persistence, `--export-links`, and current tests.
- Compared QET current ownership against the public CAE principles captured in `CAE_Reference_Matrix.md`.
- Documented options A-D without selecting an architecture: status quo extension, derived Contact/CrossRef service, dedicated `ContactAssignment` domain model, or defer into larger Device/Function core.

Open decisions:
- Whether `group_index` remains a compatibility detail or becomes/feeds a long-term assignment key.
- Whether contact assignment is owned by master, slave, link object, or independent domain object.
- Whether `--export-links` should later expose group/contact/capacity facts or a separate export/report surface should be added.

Next recommended task:
- Prepare the next test-only stabilization slice for Master/Slave link persistence and `group_index` observability, or perform the parallel ownership decision-prep for Cable/Core/Potential.

## 2026-09-21 - Master/Slave Link Persistence Test Slice

Completed:
- Added `tst_master_slave_links` under `tests/qttest/`.
- Replaced the oversized `tests/qttest/fixtures/master_slave_links_group_index.qet` with `tests/qttest/fixtures/master_slave_links_group_index_minimal.qet`.
- No existing library element/example with `slaveContactGroups` was found; the new fixture is test-only and constructed from the QET `ElementData` parser/writer structure for `slaveContactGroups`.
- Registered the new test in `tests/qttest/CMakeLists.txt`.

Verification target:
- `--resave` must preserve the selected master link UUIDs and `group_index` attributes.
- `--export-links` must report the selected `KMS` master row as linked and the fixture must have no `UNRESOLVED` status.

Open risk:
- The minimal `slaveContactGroups` fixture is not copied from an existing library definition; it is based on QET parser/writer evidence and verified by load/resave/export tests.

## 2026-09-21 - Master/Slave Test Review and Contact/CrossRef Read-only Spec

Completed:
- Reviewed corrected `tst_master_slave_links`, minimal fixture, CMake registration, and Brain/Progress consistency.
- Re-ran focused verification for the new Master/Slave test plus affected CLI tests.
- Created `Spec_Contact_CrossRef_ReadOnly_Service.md` as a non-implementing specification.

Review result:
- No blocking findings found in the corrected test slice.
- Fixture now contains embedded `slaveContactGroups` and `group_index` values that semantically reference those groups.
- The test remains CLI/XML-only and does not claim CrossRef rendering or GUI behavior.

Specification result:
- Defined a read-only projection service over existing `Element` links, `ElementData`, `group_index`, `ContactUsage`, XML link persistence, and project state.
- Explicitly kept ownership, persistence, rendering, and Device/Function core decisions out of scope.

Next recommended task:
- Review the read-only service specification. If accepted, decide whether to prototype the narrow projection service as test-covered code without persistence or UI changes.

## 2026-09-21 - Read-only Service Spec Review

Completed:
- Reviewed `Spec_Contact_CrossRef_ReadOnly_Service.md` against QET code findings, `Decision_Contact_CrossRef_Ownership.md`, `CAE_Reference_Matrix.md`, Brain, and Progress.
- Confirmed the spec remains read-only and does not approve ownership, persistence, UI/rendering, Device/Function core, CLI/export, or migration changes.

Review result:
- No blocking findings.
- Prototype boundary is testable: derive deterministic projection records from `QETProject`/`Element`/`ElementData`/`group_index`/`ContactUsage` without writes or UI dependencies.

Next recommended task:
- If explicitly approved, implement a narrow read-only projection prototype with direct QtTest coverage. Do not change XML persistence, CrossRef rendering, CLI/export behavior, or Device/Function ownership in that slice.

## Current Phase
Private MAM CAE feature work on the existing fork.

Current product decision:
- The fork is being shaped into a productive MAM CAE system inspired by EPLAN/WSCAD-style workflows, not into a legacy-compatible generic QET distribution.
- There are no existing MAM legacy projects that must preserve historical QET page defaults. Deterministic save/load behavior remains important, but deliberate MAM defaults may replace old QET defaults when they improve the CAE workflow.

## 2026-09-27 - Schematic Template Priority

User priority: schematic page templates must be a first-class CAE workstream and should follow established EPLAN presentation principles. This is a visible product gap, not merely a title-block styling task.

Current evidence:
- QElectroTech already has reusable title-block templates in `titleblocks/` and per-page properties for title block, border, row/column grid, and page numbering. Existing templates include generic QET/DIN/ISO samples; this is infrastructure, not yet a MAM CAE template system.
- EPLAN's current public help describes page templates through page type, plot frame, form, grid, orientation/paper format, page description, and structured page identifiers. The frame can define the usable drawing area and coordinate grid; page properties can override project defaults.
- EPLAN's public help also describes project/page structure identifiers and page types. These are reference principles for our design, not a request to copy proprietary forms or internal formats.

CAE status at this point:
- Present: QET primitives for title blocks, border/grid, page properties and project/page metadata. The MAM frame implementation is not accepted: the user rejected the last attempt as not matching the supplied whole-sheet reference. `Rahmenvorlage.png` in the project root is the binding source for correction.
- Partial: coil contact mirror has a code-level fixed below-coil placement slice; interactive rendering/import remains unverified. Device library has initial Eaton relay/contactor profiles and generic contact symbols.
- Missing: whole-sheet A3 frame correction against `Rahmenvorlage.png`, integration as a dependable new-project default, protection-device side contact mirrors, complete visible/verified contact-reference workflow, and a unified user-facing CAE workflow.

## CAE Arbeitsablauf und Abnahmepunkte

Das Ziel ist ein zusammenhängender Schaltplan-Arbeitsablauf. Jede Stufe muss im Beispielprojekt sichtbar funktionieren, bevor darauf die nächste aufbaut.

1. **Arbeitsblatt fertigstellen.** Den gesamten A3-Rahmen anhand `Rahmenvorlage.png` korrigieren, einschließlich Zeichnungsfeld, Koordinatenstreifen und Schriftfeld. Die letzte Umsetzung wurde zurückgewiesen und ist nicht abgenommen.
2. **Strompfade und Potentiale lesbar machen.** Eindeutige, automatisch gepflegte Strompfadnummern von Seiten-/Rasterkoordinaten und Leiternummern unterscheiden; Potentiale über Seiten hinweg verweisen. Ergebnis: ein Anschluss ist über Pfad und Potential schnell auffindbar.
3. **Betriebsmittel und Querverweise stabilisieren.** BMK eindeutig vergeben und gleiche Gerätefunktionen auf anderen Seiten automatisch miteinander verknüpfen. Ergebnis: vom Betriebsmittel oder Verweis gelangt man zur passenden Gegenstelle, auch nach Änderungen am Plan.
4. **Zusammengehörige Klemmen im Schaltplan abbilden.** Klemmen als geordnete Leiste mit stabilen Nummern, Anschlüssen und Gegenstellen führen. Ergebnis: Änderungen an einer Klemme bleiben der richtigen Leiste und Verbindung zugeordnet.
5. **Kontaktspiegel vervollständigen.** Spulen zeigen alle zugehörigen Schließer und Öffner unterhalb; Schutzgeräte ihre Hilfskontakte seitlich; SPS-Ein-/Ausgänge ihre verbundenen Kanäle und Verweise. Ergebnis: Gerät und alle verteilten Funktionen sind im Schaltplan nachvollziehbar.
6. **Gemeinsames Beispiel abnehmen.** Einen kleinen Plan mit Versorgung, SPS-E/A, Schützspule und Kontakten, Potentialwechsel und Klemmenleiste auf mehreren Seiten aufbauen. Prüfen, ob Änderungen an BMK, Pfad oder Klemme alle sichtbaren Verweise aktuell halten.

**Spätere Auswertungen:** Betriebsmittel-/BMK-Übersicht, Klemmenplan und physischer Aufbauplan. Sie bauen auf den geprüften Schaltplandaten auf und sind nicht Teil des ersten Grundlagenumfangs.

### 2026-09-28 - Bindende Ganzblattreferenz und zurückgewiesene Umsetzung
- Verbindliche Bilddatei ist `/Volumes/MAM Home/Github/QElectrotech_MAM/Rahmenvorlage.png` (4532 × 3210 px); ältere Desktop-Screenshots und frühere Rekonstruktionen sind nachrangig.
- Der Nutzer hat die letzte Rahmenumsetzung ausdrücklich als falsch/zurückgewiesen bezeichnet. Keine frühere Headless-Ansicht oder Formulierung „visuell geprüft“ bedeutet Abnahme.
- Vor jeder Rahmenänderung: PNG visuell und maßlich untersuchen, den aktuellen QET-Beispielrahmen frisch exportieren, Ganzblattabweichungen konkret benennen. Headless PNG/PDF zählt nur als Strukturprüfung, nie als GUI-Abnahme.
- Erster Ganzblattvergleich: Vorlage hat oben 0–9, keinen separaten Titel-/Überschriftenstreifen, keine A–F-Seitenleisten und einen durchgehenden Zeichnungsbereich bis zum unteren Schriftfeld. Der aktuelle Export zeigt dagegen „Klemmenplan“ im zusätzlichen oberen Band und A–F links/rechts. Vorlage hat unmittelbar oberhalb des Schriftfelds links/rechts die Nachbarseitenzahlen 1/4; Schriftfeldgruppen und Feldproportionen weichen ebenfalls sichtbar ab.
- Direkt oberhalb des Schriftfeldblocks stehen Seitenverweise: links die vorherige Seitenzahl, rechts die nächste Seitenzahl. Diese müssen dynamisch aus der Projektseitenfolge ermittelt werden (erste/letzte Seite entsprechend ohne vorhandenen Nachbarn behandeln), nicht als feste Titelblocktexte.
- Jedes einzelne Feld des Schriftfelds und der Seitenverweise erhält eine eigene QET-Variable, damit es projekt-/seitenbezogen befüllt werden kann. Dazu zählen auch vorherige und nächste Seitenzahl; Felder ohne Wert bleiben als leere bzw. bewusst definierte Ausgabe darstellbar.
- Bei der späteren Verfeinerung Feldgrenzen und Proportionen anhand des Originalbildes messen; nicht erneut den gesamten Rahmenentwurf aufrollen. Das Bild ist eine Screenshotreferenz, keine native EPLAN-Formdatei.

### 2026-09-28 - Frühere A3-Umsetzung (zurückgewiesen, nicht abgenommen)
- Die damalige Umsetzung wurde vom Nutzer zurückgewiesen und ist nicht abgenommen. Die Aussage, der Schriftfeldblock sei anhand der maßgeblichen vollständigen Referenz umgesetzt, war verfrüht; aktueller Vergleich erfolgt ausschließlich gegen `Rahmenvorlage.png`.
- Einzelne Inhaltsfelder verwenden jeweils eigene Projekt-/Seitenvariablen. Die sichtbaren festen Feldüberschriften bleiben Bestandteil des Rahmens; der Firmenlogo-Platz ist als QET-Logofeld vorgesehen und noch ohne Hersteller-/Kundenlogo.
- Vorherige und nächste Blattangabe sind im Randstreifen über dem Schriftfeld an `%previous-folio-num` und `%next-folio-num` gekoppelt. QET aktualisiert diese Werte aus der Projektseitenfolge. Die Einseiten-Vorschau zeigt diese Felder erwartungsgemäß leer; ein echter mehrseitiger visueller Nachweis ist noch offen.
- Im MAM-Rahmen die untere 0-9-Leiste entfernt; die Zahlen direkt über dem Schriftfeld sind Nachbarseitenverweise, nicht wiederholte Koordinaten.
- Den Schriftfeldblock auf volle Rahmenbreite korrigiert und die Rasterhöhe so angepasst, dass der PNG-/PDF-Export wieder A3 quer ergibt. Headless `pdfinfo` meldet 1191 × 840 pt (A3); das PNG wurde visuell geprüft.
- QET wurde damals erfolgreich gebaut. Die neue Option `border="false"` für einzelne Schriftfeldzellen ermöglicht randlose Seitenverweisfelder und wird in PDF/QET-Zeichnungsausgabe sowie DXF berücksichtigt. Diese technische Prüfung ist keine visuelle Abnahme.
- Die damalige Referenz war nicht die nun verbindliche `Rahmenvorlage.png`. Typografie, Feldtexte und Proportionen sind anhand der bindenden Datei neu zu vergleichen; Firmenlogo/Logoasset ist nicht Teil der gelieferten PNG.

Do not treat the template as a decorative title block alone. Keep its remaining lower-block refinement separate from the prioritized schematic-logic work; do not begin a broad UI redesign in these slices.

Clarification after EPLAN numbering check:
- EPLAN's plot-frame grid labels normally start at 0; its documented start value can change that. This is separate from page numbers and path numbering. Path numbering has its own project setting and can be page-by-page, project-wide, or by structure identifier.
- QET's standard diagram border currently stores the number of columns/rows and their sizes, but the inspected template files and `BorderTitleBlock` model do not expose EPLAN-like start values or independently configurable row/column label formats. The earlier user observation about numbering starting at 0 or 1 should therefore be made explicit in the MAM template requirements.
- Do not conflate sheet/page numbering with coordinate-grid or electrical path references when designing the template.

Page size and symbol-grid finding:
- QET's diagram editor default placement/snap grid is 10 x 10 scene units (`Diagram::xGrid` / `yGrid`); fine keyboard movement is 1 unit. The grid is currently application/editor settings, not visibly tied to A3/A4 template size.
- QET library symbols declare geometry and terminal coordinates independently in `.elmt` files. A symbol's connection points therefore must be checked against the chosen placement grid; changing paper size alone cannot make incompatible connection points snap correctly.
- EPLAN documents 4 mm as the standard grid for electrical schematics. This does not establish that the MAM drawing sheet must always be A3. A3 is a plausible working sheet because it offers more usable space, but choose the sheet size only after validating that the standard symbols and their terminals align on a declared CAE grid.
- The MAM template decision must explicitly set paper size, usable drawing area, placement/snap grid, and library symbol connection-point compatibility as one coordinated standard; visually verify representative symbols and wiring on the printed/exported sheet.

### 2026-09-27 update - MAM A3 frame prototype (superseded, not accepted)

Completed since the planning note above:
- Added an editable QET border model for the extra heading band, column-header height, and row-header width; these values persist in project XML and application settings.
- Added the MAM EPLAN-style A3 landscape sample frame: 10 columns labeled 0-9, 8 drawing rows, heading band below the column labels, and an 11-column/3-row variable title block. The sample frame with 5 mm page margins measures 420 x 297 mm overall.
- Added `examples/MAM_EPLAN_A3_Klemmenplan.qet` as a blank project with the frame/template embedded, and `titleblocks/MAM_EPLAN_Klemmenplan_A3.titleblock` as the reusable title-block source. Custom title-block values, including `form-id`, are editable variables.
- Verified the application target builds, the project and template XML parse, and QET's headless PNG export succeeds. Visually inspected the export. It shows the frame, 0-9 columns, title band, and title block.

Still open:
- The sample project embeds the frame, but a MAM default for newly created projects has not been wired up.
- The title-block dimensions are a first reconstruction from the public reference image, not a measured or certified EPLAN frame. Check the print margins and text size against the user's reference before treating it as production-ready.
- GUI import/editing was not tested in this run; the visual check used QET's headless export.
- The new title band has text values, but DXF export currently draws its rectangle without the title/form-ID text.

### 2026-09-27 update - coil contact mirror

Completed:
- Coil masters now force a contact mirror below the coil even when a saved project preference requests the old plain cross-reference view.
- When the master declares contact groups, the mirror shows every group in the declared order. Linked slave contacts occupy their assigned `group_index`; groups without a placed slave remain visible with the declared terminal numbers.
- Added `examples/MAM_EPLAN_Kontaktspiegel.qet`, a small A3 example with an Eaton 22E master, two linked NO/NC slaves, and two unplaced contact slots.
- Built QET and rendered the example to PNG. The visual check showed all four contacts below the coil and coordinate references for the two linked slaves, including with an explicit legacy `displayhas="cross"` project default. A second render with no slave elements showed all four declared contact slots and terminal numbers below the coil.

Still open:
- This validates rendering and saved master/slave links through QET's headless project load/export path. Interactive GUI linking, editing, moving, and printing still need a manual QET GUI check.
- The example exercises one 22E combination. The remaining Eaton combinations, changeover contacts, and multi-pole power contacts need coverage before claiming every library profile renders correctly.

### 2026-09-27 update - A3 scale and contact mirror typography

- Unified QET's fallback family for diagram, dynamic, and independent text with Arial; this Mac has Arial installed. Removed the explicit generic sans-serif style hint, though Qt still logs a `Sans Serif` alias warning during headless export.
- Increased coil contact-mirror reference text to 8 pt and terminal labels to 7 pt. Moved coil-mirror terminal labels below the contact strokes and allotted enough vertical space so the numbers no longer cross the symbols.
- The high-resolution print check exposed that the A3 example's frame dimensions were about one quarter of A3. Scaled both MAM A3 examples' border grid and the title-block rows; updated contact-example element positions to the same grid so automatic references remain valid.
- Rebuilt QET, passed `git diff --check`, and verified the contact example exports as a 1588 x 1120 PNG and a one-page PDF reported as 1191 x 840 pt (A3 landscape). The mirror shows the linked references and unobstructed terminal labels.

Still open:
- Custom title-block placeholders need actual project values; without them, QET displays unresolved placeholders in the sample.
- GUI font settings can override source defaults. The native QET dialog and a physical/GUI print preview have not been checked in this pass.
- GUI verification is actively blocked by the user's macOS crash reports. The latest report (`2026-09-28 02:32:10`, PID 47009, incident `EF6A0C32-56F3-488A-AD84-9FF32EEE7B13`) is QET 0.200.1 ARM64 on macOS 26.6.1, `EXC_CRASH (SIGABRT)`, `abort()` in `HIServices::_RegisterApplication`, through AppKit menu-bar initialization and Qt `libqcocoa`, before `QApplication`/QET project processing. A GUI-style `--help` invocation reproduced the launch crash; successful headless PNG/PDF export is not GUI verification.

## Current Objective
Build a usable CAE library for relay and contactor devices, beginning with complete coil contact inventories and linked NO/NC/changeover symbols so the previously implemented contact mirror shows every declared contact below the coil.

## Completed
- Local repository inspected non-destructively on 2026-09-21.
- Current branch verified as `master`.
- Current commit verified as `d7052e396b42c2b7b14b6b2f7aecb5d0a0f54cd0`.
- `origin` verified as `https://github.com/WeltenbummlerKA/qelectrotech.git`.
- Official project source verified from project/GitHub evidence as `https://github.com/qelectrotech/qelectrotech-source-mirror.git`.
- `upstream` configured to `https://github.com/qelectrotech/qelectrotech-source-mirror.git`.
- Homebrew build dependencies available: `cmake 4.4.3`, `qt 6.11.2`, `sqlite 3.53.4`, `ninja 1.13.2`.
- Out-of-source CMake configure completed in `build/baseline` with `-DBUILD_WITH_KF=OFF`.
- Baseline build completed successfully and produced `build/baseline/qelectrotech.app`.
- CTest completed successfully: 11/11 tests passed.
- Non-interactive application checks completed: `--help` and `--version` both exited successfully; version output is `0.200.1-dev`.
- Initial read-only source survey completed for project/folio/element/link/contact/terminal/conductor/database/export/undo areas.
- `Brain.md` updated with first-pass architecture findings and open questions.
- Read-only end-to-end trace completed for element definition metadata, placed element persistence, UUID/group-index linking, contact usage/capacity, CrossRef rendering, undo command path, save/load, and current tests.
- `Brain.md` updated with Phase 2 master/slave/contact trace findings.
- Read-only end-to-end trace completed for graphical terminals, conductors, potential traversal, terminal strips, cable/bus fields, wiring/net exports, XML persistence, undo command surfaces, and current test coverage.
- `Brain.md` updated with Phase 3 terminal/cable/potential trace findings.
- Read-only end-to-end trace completed for project autonum schemas, formula resolution, sequence/counter state, element/conductor/terminal numbering, conductor text propagation, undo/redo, XML persistence, database updates, and export impact.
- `Brain.md` updated with Phase 4 numbering/autonum/conductor text trace findings.
- Read-only end-to-end trace completed for `.qet` XML file structure, project/diagram/element/conductor/terminal-strip/autonum load/save lifecycle, version/legacy paths, UUID fallbacks, deterministic save ordering, database rebuild timing, export equivalence, and current test coverage.
- `Brain.md` updated with Phase 5 XML/load-save compatibility trace findings.
- Read-only regression-test map completed for master/slave/contact mirror, terminal/potential/wiring, autonum/undo, XML round-trip/legacy, DB/export equivalence, terminal strips, and cable/conductor fields.
- `Brain.md` updated with Phase 6 test coverage and risk findings.
- Read-only fixture specification completed for XML/roundtrip/legacy, export equivalence, master/slave/contacts, terminal strip/bridges, autonum/undo, and cable/conductor fields.
- `Brain.md` updated with Phase 7 fixture/test-design findings.
- Read-only expectation/normalization rules completed for future regression tests across XML, CSV, JSON, SVG/PDF, CLI environment, platform handling, and large smoke examples.
- `Brain.md` updated with Phase 8 test-normalization findings.
- Read-only non-implementing regression-test backlog completed for XML/roundtrip/legacy, export equivalence, master/slave/contact mirror, terminal/potential/wiring, terminal strips/bridges, autonum/undo, cable/conductor fields, and large smoke examples.
- `Brain.md` updated with Phase 9 prioritized test-plan backlog.
- Read-only implementation sequence and review checklist completed for the first future test-only infrastructure slice.
- `Brain.md` updated with Phase 10 first-slice implementation/review findings.
- Implemented `tst_cli_roundtrip_xml` as a test-only QtTest under `tests/qttest/`.
- Registered `tst_cli_roundtrip_xml` in `tests/qttest/CMakeLists.txt` following the existing `tst_conductorselfretrace` binary/CTest pattern.
- The new test uses existing fixture `tests/qttest/fixtures/qet_bug_repro_resaved.qet`, runs two `--resave` passes in `QTemporaryDir`, sets `QT_QPA_PLATFORM=offscreen` for child CLI processes, parses XML with Qt DOM, and compares a normalized project tree.
- Targeted build of `tst_cli_roundtrip_xml` completed successfully.
- Targeted CTest verification passed for `tst_cli_roundtrip_xml`.
- Regression CTest verification passed for `tst_conductorselfretrace` and `tst_cli_roundtrip_xml` together.
- Review completed for `tests/qttest/tst_cli_roundtrip_xml.cpp`, `tests/qttest/CMakeLists.txt`, and the Progress/Brain entries.
- Review checklist result: no production code changes found, no existing fixtures changed, deterministic temp output via `QTemporaryDir`, offscreen child CLI processes, no GUI assumption, no large golden dumps, focused CTest integration.
- Re-run verification after review passed for `tst_cli_roundtrip_xml` and the affected short CLI test set.
- Next P0 slice defined as a planning item only: export-equivalence helpers and a minimal fixture for `--export-nets`, `--export-wiring`, `--export-cables`, `--export-wires`, `--export-bom`, and `--info`.
- Implemented `tst_cli_export_equivalence` as a test-only QtTest under `tests/qttest/`.
- Added new fixture later consolidated as `tests/qttest/fixtures/workflow_exports_minimal.qet`, derived from the existing compact fixture but with stable conductor numbers `W005`-`W011` and non-empty cable/color/section/function fields.
- Registered `tst_cli_export_equivalence` in `tests/qttest/CMakeLists.txt`.
- The new test runs `--export-nets`, `--export-wiring`, `--export-cables`, `--export-wires`, `--export-bom`, and `--info` into `QTemporaryDir`, parses JSON/CSV semantically, normalizes BOM/quotes/line endings/order, and compares stable core facts.
- Targeted build and CTest verification passed for `tst_cli_export_equivalence`.
- Affected CLI-test regression verification passed for `tst_conductorselfretrace`, `tst_cli_roundtrip_xml`, and `tst_cli_export_equivalence`.
- Review completed for `tests/qttest/tst_cli_export_equivalence.cpp`, the workflow export fixture, `tests/qttest/CMakeLists.txt`, and Progress/Brain entries.
- Review checklist result: test-only scope, no production changes, stable fixture semantics for asserted fields, `QTemporaryDir`, offscreen CLI execution, semantic CSV/JSON parsing, no large golden dumps, focused CTest integration, and no existing fixture changes.
- Re-run verification after review passed for `tst_cli_export_equivalence` and the affected short CLI test set.
- Next helper-extraction task defined as a planning item only.
- Added test-only helper `tests/qttest/cli_test_utils.h`.
- Extracted shared CLI runner, file reader, JSON object reader, semicolon CSV parser, and CSV column set helper from `tst_cli_roundtrip_xml.cpp` and `tst_cli_export_equivalence.cpp`.
- Kept XML canonicalization local in `tst_cli_roundtrip_xml.cpp` because it is specific to first/second `--resave` comparison policy.
- Updated `tst_cli_roundtrip_xml.cpp` and `tst_cli_export_equivalence.cpp` to use the shared helper without changing asserted behavior.
- Targeted build passed for `tst_cli_roundtrip_xml` and `tst_cli_export_equivalence`.
- Affected CLI-test regression verification passed for `tst_conductorselfretrace`, `tst_cli_roundtrip_xml`, and `tst_cli_export_equivalence`.
- Review completed for `tests/qttest/cli_test_utils.h`, `tst_cli_roundtrip_xml.cpp`, `tst_cli_export_equivalence.cpp`, qttest CMake registration, and Progress/Brain entries.
- Review checklist result: helper remains test-only and narrow; offscreen/temp-dir behavior preserved; CSV/JSON helpers retain existing semantics; XML policy remains local; no production or fixture changes found.
- Re-run verification after review passed for both CLI tests plus `tst_conductorselfretrace`.
- Next P0 domain-slice options A/B documented, with technical recommendation to implement Terminal/Potential export coverage first.
- Implemented `tst_terminal_potential_exports` as a test-only QtTest under `tests/qttest/`.
- Reused the shared workflow export fixture for stable terminal/potential export assertions.
- Registered `tst_terminal_potential_exports` in `tests/qttest/CMakeLists.txt`.
- The new test runs `--export-nets`, `--export-wiring`, `--export-wires`, and `--info` into `QTemporaryDir`, using `cli_test_utils.h` for CLI/CSV/JSON helpers.
- Assertions compare stable wire numbers, exact expected live-net terminal sets, wiring endpoint terminal pairs, conductor count, element count, and wire-number export set.
- Targeted build and CTest verification passed for `tst_terminal_potential_exports`.
- Affected CLI-test regression verification passed for `tst_conductorselfretrace`, `tst_cli_roundtrip_xml`, `tst_cli_export_equivalence`, and `tst_terminal_potential_exports`.
- Completed and committed the Contact Projection, PLC IO Projection, Duplicate Validation, and PLC Warning-Spec/-Extension slices through `86c5fc9076d295044c41f489e95162b095dfdc8a`.
- PLC warnings remain read-only and do not change UI, persistence, XML schema, Device/Core ownership, or migration behavior.
- Upstream PR #1058 was closed and not merged; the fork work is no longer being tracked as the current upstream contribution path.
- `origin/mam/qet-projections-runtime-qa` remains the secured fork branch state at `7efd85c52ffc14c96870ea50f968213d976e351f`.
- Added a small read-only PLC stale-copy warning slice on 2026-09-26: `PlcIoProjectionService` now flags linked slave `plc_*` display/formula fields that differ from the resolved master IO row.
- Extended `tst_plcioprojectionservice` with focused regression coverage for warning-only stale slave `plc_address` evidence.
- Added the next smallest read-only PLC slave-terminal-count evidence slice on 2026-09-26: linked PLC slave projections now expose observed slave terminal count and warn when it is lower than the PLC IO `terminalCount`.
- Extended `tst_plcioprojectionservice` coverage for the warning-only insufficient slave-terminal-count diagnostic.
- Re-audited Contact/CrossRef and PLC IO projection diagnostics on 2026-09-26.
  Warning/flag naming and messages are deterministic and intentionally read-only; PLC projections still use `PlcMasterData::ios` plus `group_index` as authority and treat slave `plc_*` values as stale-copy evidence only.
  Existing focused QtTests cover contact duplicate/missing/out-of-range/type-mismatch diagnostics and PLC duplicate/out-of-range/empty-address/terminal-label/stale-copy/slave-terminal-count diagnostics.
- Added a small terminal/potential/export-equivalence coverage slice on 2026-09-26: `tst_terminal_potential_exports` now includes `--export-cables` and checks its terminal endpoint-pair multiset against the same workflow fixture used for net/wiring/wire export assertions.
  This is test-only coverage and does not change production export logic, UI, persistence, XML schema, Device/Core ownership, or runtime/package behavior.
- Added the next smallest terminal/potential/export-equivalence coverage slice on 2026-09-26: `tst_terminal_potential_exports` now compares full cable endpoint pairs (`Composant` + `Borne`) against the database-backed `--export-wiring` endpoint pairs for the same fixture.
  This is test-only coverage and keeps the cable exporter equivalence check within existing CLI/QtTest boundaries without production, UI, persistence, XML schema, Device/Core, runtime, or package changes.
- Added the next small terminal/potential/export-equivalence coverage slice on 2026-09-26: `tst_terminal_potential_exports` now verifies that the database-backed `--export-wiring` rows expose one non-empty, unique `conductor_uuid` per expected conductor in the workflow fixture.
  This is test-only evidence for conductor identity coverage and does not change production export logic, UI, persistence, XML schema, Device/Core ownership, runtime, or package behavior.
- Added another small terminal/potential/export-equivalence coverage slice on 2026-09-26: `tst_terminal_potential_exports` now pins the expected `--export-wiring` and `--export-cables` CSV headers used by the endpoint/identity comparisons.
  This remains test-only export contract coverage and does not change production export logic, UI, persistence, XML schema, Device/Core ownership, runtime, or package behavior.
- Added terminal-strip source-of-truth/read-only decision evidence on 2026-09-26 in `Decision_Terminal_Strip_ReadOnly_Boundary.md`.
  Current evidence keeps terminal strips project-owned through `QETProject` and `TerminalStrip`, with `RealTerminal` membership resolved by placed terminal `Element` UUIDs and bridges resolved back through loaded strip real terminals.
  This is documentation only; no product code, UI, persistence, XML schema, Device/Core, runtime, or package behavior changed.
- Added a small terminal-strip read-only regression slice on 2026-09-26: `tst_terminalstrip_roundtrip` uses a clearly synthetic/provisional fixture derived from the documented `TerminalStrip`/`PhysicalTerminal`/`RealTerminal`/`TerminalStripBridge` XML shape and an embedded terminal-block element based on the existing terminal definition structure.
  The test verifies that `--resave` preserves project-level terminal-strip membership and bridge UUID/color references across first and second saves.
  This is test-only coverage and does not change product code, UI, persistence format, XML schema, Device/Core ownership, runtime, or packaging behavior.
- Added the next smallest terminal-strip regression slice on 2026-09-26: `tst_terminalstrip_roundtrip` now also verifies that the strip data fields `installation`, `location`, `name`, `comment`, and `description` survive fixture load, first `--resave`, and second `--resave`.
  This remains test-only read-only coverage over the existing terminal-strip XML/data surface and does not change product code, UI, persistence format, XML schema, Device/Core ownership, runtime, or packaging behavior.
- Added another small terminal-strip regression slice on 2026-09-26: `tst_terminalstrip_roundtrip` now verifies physical-terminal layout grouping, not only the flattened real-terminal membership list.
  The fixture still covers only existing project-level terminal-strip XML load/resave preservation and remains outside product code, UI, persistence format, XML schema, Device/Core ownership, runtime, and packaging.
- Added the first MAM-specific read-only report/export slice on 2026-09-26: `--export-mam-plc-io` writes a semicolon CSV from `PlcIoProjectionService` with PLC master row facts, linked slave evidence, status, and warning text.
  The export consumes the existing read-only PLC projection service and does not change UI, persistence, XML schema, Device/Core ownership, runtime, or packaging.
  Focused CLI coverage is in `tst_mam_plc_io_export`, using a temporary PLC variant derived from the existing master/slave fixture.
- Added `Spec_MAM_Report_ReadOnly.md` on 2026-09-26 as the short field/audience decision for the combined MAM report family.
  Boundary: read-only CLI/CSV first, no UI, persistence, XML schema, Device/Core, runtime, packaging, or repair behavior.
  The next small implementation slice is a Contact/CrossRef CSV over `ContactCrossRefProjectionService`.
- Added the next MAM-specific read-only report/export slice on 2026-09-26: `--export-mam-contact-crossref` writes a semicolon CSV from `ContactCrossRefProjectionService` with master/slave identity, `group_index`, group metadata, slave contact metadata, status, and warning text.
  The export consumes the existing read-only Contact/CrossRef projection service and does not change UI, persistence, XML schema, Device/Core ownership, runtime, packaging, or repair behavior.
  Focused CLI coverage is in `tst_mam_contact_crossref_export`, using the existing master/slave group-index fixture.
- Added a small warning-case coverage slice for `--export-mam-contact-crossref` on 2026-09-26: the CLI test now derives a duplicate `group_index` variant from the existing fixture and verifies exported `WARNING` status, duplicate flag, and warning text.
  This is test-only coverage over existing `ContactCrossRefProjectionService` diagnostics; no product, UI, persistence, XML schema, Device/Core, runtime, packaging, or repair behavior changed.
- Added the next small `--export-mam-contact-crossref` warning coverage slice on 2026-09-26: the CLI test now exports missing, out-of-range, and type-mismatch `group_index` variants derived from the same fixture.
  This also remains test-only coverage over existing projection warnings; no product behavior or persisted project data changed.
- Added the Terminal/Potential MAM CSV field decision on 2026-09-26 in `Spec_MAM_Report_ReadOnly.md`.
  The selected next report is `--export-mam-terminal-potential` as a semicolon CSV over existing live conductor/potential graph evidence and conductor endpoint identities.
  It deliberately excludes terminal-strip membership/bridges, cable/core ownership, UI, persistence, XML schema, Device/Core migration, runtime, packaging, and repair behavior.
- Added the first Terminal/Potential MAM read-only export slice on 2026-09-26: `--export-mam-terminal-potential` writes one semicolon CSV row per conductor with endpoint identity and potential-group counts.
  The export uses the same live potential traversal boundary as `--export-nets` and does not change UI, persistence, XML schema, Device/Core ownership, runtime, packaging, or repair behavior.
  Focused CLI coverage is in `tst_mam_terminal_potential_export`, using the existing workflow export fixture.
- Added a small warning-case coverage slice for `--export-mam-terminal-potential` on 2026-09-26: the CLI test now derives an empty-wire-number variant from the existing workflow export fixture and verifies exported `WARNING` status, preserved endpoint/conductor evidence, and warning text.
  This remains test-only coverage over existing export-level diagnostics; no product behavior, UI, persistence, XML schema, Device/Core, runtime, packaging, or repair behavior changed.
- Validated the private MAM fork milestone on 2026-09-26 with the affected regression set green: PLC IO projection/export, Contact/CrossRef projection/export, Terminal/Potential export, terminal-strip round-trip, CLI export/XML round-trip, master/slave links, contact usage, and conductor self-retrace.
  Current MAM CSV family exists for PLC IO, Contact/CrossRef, and Terminal/Potential. Still deliberately open: full CTest, UI/workflow validation, packaging/test-bundle validation, and a combined summary report.
- Added the first combined MAM overview list on 2026-09-26: `--export-mam-summary` writes one semicolon CSV working list with separate `plc_io`, `contact_crossref`, and `terminal_potential` rows.
  It reuses the existing read-only projections/export logic and does not join unrelated domains, change UI, persistence, XML schema, Device/Core ownership, runtime, packaging, or repair behavior.
  Focused CLI coverage is in `tst_mam_summary_export`, checking the shared header plus expected contact and terminal/potential warning rows.
- Added the first user-facing MAM Summary example on 2026-09-27 to `Spec_MAM_Report_ReadOnly.md`, including the CLI command, CSV header, and representative separated row types.
  Extended `tst_mam_summary_export` to pin deterministic terminal/potential row ordering (`W005` through `W011`) from the workflow fixture. This remains read-only documentation/test coverage with no product behavior changes.
- Started the first visible CAE slice on 2026-09-27: coil contact mirrors in contact-display mode are anchored below the coil with a fixed 20-unit gap, independent of the old page/label anchor choice. A fresh installation defaults coil cross-references to contact symbols unless the user already saved an explicit preference. Coil mirrors show every declared master contact group and every linked slave; linked symbols retain their existing double-click navigation.
  The application and focused project-load test build; `tst_contactcrossrefprojectionservice` passes. The current standard symbol library does not declare model-specific free-contact inventories, so unlinked contacts appear only when the master defines those groups. Manual GUI inspection and real relay/contactor inventory coverage remain open.

## In Progress
- Coil contact mirror: first visible behavior slice implemented; interactive GUI review and model-specific free-contact inventories remain open.

## Pending
- Strompfade als erste CAE-Funktionslücke spezifizieren und umsetzen: QET-Rasterkoordinaten (Seite/Zeile/Spalte) und Potential-/Leiternummerierung sind vorhanden, ersetzen aber keine separat verwaltete Pfadnummer. Vor Implementierung festlegen: Nummerierung je Seite/Projekt/Struktur, Startwert und Verhalten beim Einfügen/Verschieben von Seiten oder Pfaden; danach Pfadbezeichnungen mit Potentialverweisen und Querverweisen verbinden. EPLAN dokumentiert diese Nummerierungsarten und eine Aktualisierung der Pfadbezeichnungen.
- Connected terminal groups are a first-basics gap: represent related terminals as one ordered group with stable designations and cross-references in the schematic. The terminal plan itself remains a later generated document.
- Later documents/views: automatic device (BMK) overview, terminal plan, and physical layout plan (Aufbauplan). Do not pull these ahead of the schematic basics above.
- Keep `mam/qet-projections-runtime-qa` as the current private/MAM fork line unless an explicit private-main branch decision is made.
- Keep upstream as a reference/source for occasional updates, not as the current contribution target. Upstream PR work is not the goal of this roadmap.
- Decide later whether and how to mark the verified baseline.
- Interactive GUI startup is partially verified: user-controlled manual `open -n build/baseline/qelectrotech.app` launch started successfully. Further smoke behavior should only run with the user present for any macOS permission prompts.
- A later optional Runtime-QA/installation slice may verify startup interactively, but it must be handled separately from PLC/CAE feature work.
- Runtime-QA/installation slice scope, if approved later: clarify the exact bundle path, perform a controlled local install or bundle launch test, let the user handle any macOS dialogs, and document the observed result.
- Dedicated deep dives still pending for UI/domain coupling, project database lifecycle, export equivalence, terminal-strip/potential integration tests, and automated regression coverage design.
- Tests for full master/slave XML round-trip, link undo/redo, group-index persistence, CrossRef click map/render behavior, and PLC link propagation remain unverified/missing in this pass.
- High-priority future regression candidates identified, but not implemented: CLI fixture matrix, XML round-trip determinism, DB/export equivalence, terminal-strip round-trip, autonum undo/redo, and master/slave link persistence/render checks.
- Future fixture creation still requires an explicit implementation phase and expected-output policy before files are added.
- Future test implementation still requires explicit approval and should start with normalization helpers before adding behavioral assertions.
- Phase 9 backlog remains a specification only; no test files, fixtures, helpers, or CMake registrations were created.
- Phase 10 first-slice plan remains a specification only; no test harness, helpers, fixtures, CMake registrations, or documentation files were created.
- Full P0 fixture matrix, export equivalence tests, master/slave tests, terminal-strip tests, autonum/undo tests, cable/conductor-field tests, PDF checks, and large smoke examples remain unimplemented.
- Interactive GUI startup is partially verified by the user-controlled `open -n build/baseline/qelectrotech.app` launch. Clean installation, embedded Qt/deploy step, codesign, packaging, and `/Applications` installation remain unverified/open.
- Full CTest suite has not been re-run after the PLC warning extensions and MAM CSV summary slice.
- Default KF/ECM build behavior remains open because the active baseline uses `-DBUILD_WITH_KF=OFF`.

## Private MAM Fork Implementation Plan

### Phase 0 - Sicherung, Branch-Strategie, Fork-Grundsatz
- Treat the branch `mam/qet-projections-runtime-qa` and the pushed state after `ac0980b3b3e8 Record private MAM fork direction` as the secured private fork baseline.
- Decide only one branch policy before new feature slices: either continue this dedicated MAM work branch or promote a private MAM mainline. Do not mix both.
- Keep upstream QElectroTech as read-only reference and occasional rebase/compare source, not as the delivery target.
- No-Go: no upstream PR plan, no contribution-roadmap, no feature work before the private branch policy is stated.

### Phase 1 - Existing Read-only Projection/Validation Stabilisieren
- Audit and tighten the current read-only Contact/CrossRef and PLC IO Projection Services before broadening semantics.
- Keep projections derived from loaded `QETProject`/`Element`/`ElementData` state; writes, repairs, XML changes, and UI behavior stay out of scope.
- Add only small warning/validation slices where current service boundaries can already express the facts.
- No-Go: no hidden mutation through read APIs, no persistence migration, no UI or command behavior change in this phase.

### Phase 2 - PLC/Contact/Terminal/Potential Semantik Vertiefen
- Deepen domain evidence in narrow order: PLC IO rows and `group_index`, contact capacity/assignment, terminal identity, conductor potentials, terminal strips, then export equivalence.
- Prefer read-only diagnostics and fixtures that compare live graph, XML-derived exports, and SQLite-derived reports.
- Keep `PlcMasterData::ios`, `ElementData`, current links, and conductor/terminal objects as observed sources until a later ownership decision is explicit.
- No-Go: no early Device/Core rewrite, no inferred vendor-specific PLC rules, no terminal-strip/potential ownership change without tests and decision note.
- Terminal-strip decision evidence now exists; a future test-only terminal-strip fixture may only cover existing project-level XML load/resave compatibility if it is derived directly from the documented code/XML structure.

### Phase 3 - MAM-specific Data, Export, Report Layer
- Add MAM-specific reports/exports as a separate layer over stable projections where possible.
- Keep report/export naming, filtering, and validation MAM-specific and private to the fork.
- Use existing CLI/export/test helper patterns before introducing new infrastructure.
- No-Go: no report/export work that silently changes core QET behavior; no mixing MAM report rules into generic upstream-facing code paths.

### Phase 4 - Kontrollierte UI-/Workflow-Integration
- Start UI/workflow changes only after the underlying projection semantics are stable and testable.
- Integrate as opt-in MAM workflow surfaces first: warnings, review views, or explicit commands, not automatic rewrites.
- Keep undo/redo, graphics rendering, and project persistence separate review topics.
- No-Go: no early UI-first implementation, no CrossRef rendering rewrite as a shortcut for domain ownership, no command behavior change without focused verification.

### Phase 5 - Lokale Runtime, Packaging, Test-Bundle Strategie
- Handle runtime/package/install work as its own slice after feature semantics are separated.
- Establish the local bundle target, Qt deployment, signing/resources status, and user-controlled macOS GUI smoke checks before any installation claim.
- Keep build/test/runtime evidence current per slice, especially because the known bundle is still a dev bundle with external Homebrew Qt dependencies.
- No-Go: do not mix packaging/runtime QA with PLC/CAE feature logic; do not treat CLI or bundle inspection as full GUI/install proof.

### Phase 6 - Spaetere Persistenz/XML/Core-Migration Nur Nach Expliziter Entscheidung
- Consider XML schema, persistence, Device/Core, ContactAssignment, PLC device model, or canonical database changes only after phases 1-4 produce stable facts and user approval.
- Prepare a decision note before any migration: current source of truth, compatibility impact, rollback path, fixture coverage, and export/report consequences.
- Keep deterministic MAM project save/load behavior as a hard acceptance criterion. Generic legacy `.qet` compatibility is useful evidence, but not a blocker for deliberate MAM-only defaults where no old MAM projects exist.
- No-Go: no speculative schema migration, no core rewrite hidden inside report/UI work, no migration without explicit decision and tests.

## Blocked
- None.

## Decisions Required
- None for the completed baseline build.

## Last Verified Build
- Date: 2026-09-21.
- Configure: `/opt/homebrew/bin/cmake -S . -B build/baseline -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_WITH_KF=OFF -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt -DCMAKE_MAKE_PROGRAM=/opt/homebrew/bin/ninja`.
- Build: `/opt/homebrew/bin/cmake --build build/baseline --parallel`.
- Result: success, generated `build/baseline/qelectrotech.app`.
- Build commit reported by CMake: `d7052e396b42c2b7b14b6b2f7aecb5d0a0f54cd0`.

## Tests
- `/opt/homebrew/bin/ctest --test-dir build/baseline --output-on-failure`: 11/11 tests passed.
- Non-interactive binary checks:
  - `build/baseline/qelectrotech.app/Contents/MacOS/qelectrotech --help`: exited 0 and printed CLI usage.
  - `build/baseline/qelectrotech.app/Contents/MacOS/qelectrotech --version`: exited 0 and printed `0.200.1-dev`.
- First test-only patch verification:
  - `/opt/homebrew/bin/cmake --build build/baseline --target tst_cli_roundtrip_xml --parallel`: passed.
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R tst_cli_roundtrip_xml --output-on-failure`: 1/1 passed.
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R "tst_(cli_roundtrip_xml|conductorselfretrace)" --output-on-failure`: 2/2 passed.
- P0 export-equivalence verification:
  - `/opt/homebrew/bin/cmake --build build/baseline --target tst_cli_export_equivalence --parallel`: passed.
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R tst_cli_export_equivalence --output-on-failure`: 1/1 passed.
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R "tst_(cli_export_equivalence|cli_roundtrip_xml|conductorselfretrace)" --output-on-failure`: 3/3 passed.
- P0 export-equivalence review verification:
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R tst_cli_export_equivalence --output-on-failure`: 1/1 passed.
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R "tst_(cli_export_equivalence|cli_roundtrip_xml|conductorselfretrace)" --output-on-failure`: 3/3 passed.
- Helper extraction verification:
  - `/opt/homebrew/bin/cmake --build build/baseline --target tst_cli_roundtrip_xml tst_cli_export_equivalence --parallel`: passed.
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R "tst_(cli_export_equivalence|cli_roundtrip_xml|conductorselfretrace)" --output-on-failure`: 3/3 passed.
- Helper extraction review verification:
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R "tst_(cli_export_equivalence|cli_roundtrip_xml|conductorselfretrace)" --output-on-failure`: 3/3 passed.
- P0 Terminal/Potential verification:
  - `/opt/homebrew/bin/cmake --build build/baseline --target tst_terminal_potential_exports --parallel`: passed.
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R tst_terminal_potential_exports --output-on-failure`: 1/1 passed.
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R "tst_(terminal_potential_exports|cli_export_equivalence|cli_roundtrip_xml|conductorselfretrace)" --output-on-failure`: 4/4 passed.
- Contact/CrossRef read-only projection prototype verification:
  - `cmake -S . -B build/baseline`: passed after qttest CMake registration changes.
  - `cmake --build build/baseline --target tst_contactcrossrefprojectionservice`: passed.
  - `cmake --build build/baseline --target qelectrotech`: passed; rebuilt `qelectrotech.app/Contents/MacOS/qelectrotech` with `sources/contactcrossrefprojectionservice.cpp`.
  - `ctest --test-dir build/baseline -R 'tst_contactcrossrefprojectionservice|tst_master_slave_links|tst_contactusage' --output-on-failure`: 3/3 passed.
  - `ctest --test-dir build/baseline -R 'tst_cli_roundtrip_xml|tst_cli_export_equivalence|tst_terminal_potential_exports|tst_master_slave_links|tst_contactcrossrefprojectionservice|tst_contactusage' --output-on-failure`: 6/6 passed.
- Contact/CrossRef read-only prototype review verification:
  - `cmake --build build/baseline --target qelectrotech tst_contactcrossrefprojectionservice`: passed; no rebuild work needed.
  - `ctest --test-dir build/baseline -R 'tst_cli_roundtrip_xml|tst_cli_export_equivalence|tst_terminal_potential_exports|tst_master_slave_links|tst_contactcrossrefprojectionservice|tst_contactusage' --output-on-failure`: 6/6 passed.
- Contact/CrossRef negative projection test-only slice verification:
  - `cmake --build build/baseline --target tst_contactcrossrefprojectionservice`: passed.
  - `ctest --test-dir build/baseline -R 'tst_contactcrossrefprojectionservice' --output-on-failure`: 1/1 passed.
  - `cmake --build build/baseline --target qelectrotech tst_contactcrossrefprojectionservice`: passed; no rebuild work needed after the focused test build.
  - `ctest --test-dir build/baseline -R 'tst_cli_roundtrip_xml|tst_cli_export_equivalence|tst_terminal_potential_exports|tst_master_slave_links|tst_contactcrossrefprojectionservice|tst_contactusage' --output-on-failure`: 6/6 passed.
- First patch review verification:
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R tst_cli_roundtrip_xml --output-on-failure`: 1/1 passed.
  - `/opt/homebrew/bin/ctest --test-dir build/baseline -R "tst_(cli_roundtrip_xml|conductorselfretrace)" --output-on-failure`: 2/2 passed.

## Analysis Evidence
- Project model: `sources/qetproject.h/.cpp`.
- Folio/page model: `sources/diagram.h/.cpp`.
- Element/symbol model: `sources/qetgraphicsitem/element.h/.cpp`, `sources/properties/elementdata.h/.cpp`.
- Master/slave/report/cross-reference model: `sources/qetgraphicsitem/masterelement.cpp`, `slaveelement.cpp`, `reportelement.cpp`, `crossrefitem.h`.
- Terminal/conductor/potential model: `sources/qetgraphicsitem/terminal.h`, `conductor.h`, `cli_export.cpp`.
- Terminal strip model: `sources/TerminalStrip/terminalstrip.h`, `terminalstripdata.h`.
- Project database/reporting: `sources/dataBase/projectdatabase.h/.cpp`, `sources/bomexport.cpp`, `sources/conductornumexport.cpp`, `sources/cli_export.cpp`.
- Undo/redo surface: `QETProject` and `Diagram` expose/use `QUndoStack`; concrete commands are under `sources/undocommand/`, `sources/TerminalStrip/UndoCommand/`, and `sources/editor/UndoCommand/`.
- Phase 2 trace: `.elmt` metadata examples under `elements/10_electric/20_manufacturers_articles/loxone/`, `sources/properties/elementdata.cpp`, `sources/qetgraphicsitem/element.cpp`, `sources/undocommand/linkelementcommand.*`, `sources/ui/linksingleelementwidget.cpp`, `sources/ui/contactgroupselectiondialog.cpp`, `sources/qetgraphicsitem/masterelement.*`, `sources/contactusage.h`, `sources/qetgraphicsitem/crossrefitem.*`, and `tests/qttest/tst_contactusage.cpp`.
- Phase 3 trace: `sources/qetgraphicsitem/terminal.*`, `sources/qetgraphicsitem/conductor.*`, `sources/TerminalStrip/realterminal.*`, `physicalterminal.*`, `terminalstrip.*`, `TerminalStrip/UndoCommand/*`, `sources/xml/terminalstripitemxml.*`, `sources/qetproject.cpp`, `sources/conductorproperties.cpp`, `sources/ui/conductorpropertieswidget.cpp`, `sources/dataBase/projectdatabase.cpp`, `sources/wiringlistexport.cpp`, `sources/cli_export.cpp`, `sources/ui/wiringlistdialog.cpp`, and `tests/qttest/tst_conductorselfretrace.cpp`.
- Phase 4 trace: `sources/autoNum/assignvariables.*`, `sources/autoNum/numerotationcontext.*`, `sources/autoNum/numerotationcontextcommands.*`, `sources/conductorautonumerotation.*`, `sources/undocommand/setautonumcontextcommand.*`, `sources/qetproject.*`, `sources/diagram.*`, `sources/qetgraphicsitem/element.cpp`, `sources/qetgraphicsitem/conductor.cpp`, `sources/ui/diagrampropertiesdialog.cpp`, `sources/ui/terminalnumberingdialog.cpp`, `sources/undocommand/changeelementinformationcommand.*`, `sources/dataBase/projectdatabase.cpp`, `sources/cli_export.cpp`, `sources/conductornumexport.cpp`, `sources/wiringlistexport.cpp`, and `tests/qttest/fixtures/qet_bug_repro_resaved.qet`.
- Phase 5 trace: `sources/qetproject.*`, `sources/diagram.*`, `sources/qet.cpp`, `sources/qetversion.*`, `sources/qetgraphicsitem/element.cpp`, `sources/qetgraphicsitem/conductor.cpp`, `sources/TerminalStrip/terminalstrip.cpp`, `sources/xml/terminalstripitemxml.cpp`, `sources/autoNum/numerotationcontext.cpp`, `sources/autoNum/assignvariables.cpp`, `sources/dataBase/projectdatabase.*`, `sources/cli_export.cpp`, `sources/wiringlistexport.cpp`, `tests/qttest/tst_diagramsortkeys.cpp`, `tests/qttest/tst_conductorselfretrace.cpp`, and `tests/qttest/fixtures/qet_bug_repro_resaved.qet`.
- Phase 6 test map: `CMakeLists.txt`, `tests/CMakeLists.txt`, `tests/qttest/CMakeLists.txt`, `tests/qttest/tst_contactusage.cpp`, `tests/qttest/tst_conductorselfretrace.cpp`, `tests/qttest/tst_diagramsortkeys.cpp`, `tests/qttest/tst_smart_device.cpp`, `tests/qttest/tst_qetstrings.cpp`, `tests/qttest/fixtures/qet_bug_repro_resaved.qet`, `tests/modal-quit-regression/*`, `tests/ipc-regression/README.md`, `sources/cli_export.cpp`, `sources/wiringlistexport.cpp`, `sources/conductornumexport.cpp`, and `sources/conductorproperties.cpp`.
- Phase 7 fixture specification: `examples/*.qet`, `tests/qttest/fixtures/qet_bug_repro_resaved.qet`, `elements/10_electric/10_allpole/310_relays_contactors_contacts/`, `elements/10_electric/10_allpole/130_terminals_terminal_strips/`, `elements/10_electric/10_allpole/120_cables_wiring/`, `sources/cli_export.cpp`, and existing QtTest/CTest files.
- Phase 8 normalization rules: `sources/qet.cpp`, `sources/cli_export.cpp`, `sources/wiringlistexport.cpp`, `sources/conductornumexport.cpp`, `sources/bomexport.cpp`, `tests/qttest/tst_conductorselfretrace.cpp`, `tests/modal-quit-regression/*`, and `tests/ipc-regression/*`.
- Phase 9 test-plan backlog: `Brain.md` Phase 6-8 findings, `tests/qttest/CMakeLists.txt`, `tests/qttest/tst_contactusage.cpp`, `tests/qttest/tst_conductorselfretrace.cpp`, `tests/qttest/tst_diagramsortkeys.cpp`, `tests/qttest/tst_smart_device.cpp`, `tests/qttest/fixtures/qet_bug_repro_resaved.qet`, `tests/modal-quit-regression/*`, `tests/ipc-regression/README.md`, `sources/cli_export.cpp`, `sources/wiringlistexport.cpp`, `sources/conductornumexport.cpp`, and high-risk source areas documented in earlier phases.
- Phase 10 implementation/review plan: `tests/qttest/CMakeLists.txt`, `tests/qttest/tst_conductorselfretrace.cpp`, `tests/qttest/fixtures/qet_bug_repro_resaved.qet`, current CTest/QtTest registration pattern, and Phase 7-9 fixture/helper/backlog findings in `Brain.md`.
- First test-only patch implementation: `tests/qttest/tst_cli_roundtrip_xml.cpp` and `tests/qttest/CMakeLists.txt`.
- P0 export-equivalence implementation: `tests/qttest/tst_cli_export_equivalence.cpp`, `tests/qttest/fixtures/workflow_exports_minimal.qet`, and `tests/qttest/CMakeLists.txt`.
- Helper extraction implementation: `tests/qttest/cli_test_utils.h`, `tests/qttest/tst_cli_roundtrip_xml.cpp`, and `tests/qttest/tst_cli_export_equivalence.cpp`.
- P0 Terminal/Potential implementation: `tests/qttest/tst_terminal_potential_exports.cpp`, shared `tests/qttest/fixtures/workflow_exports_minimal.qet`, and `tests/qttest/CMakeLists.txt`.
- Contact/CrossRef read-only projection prototype: `sources/contactcrossrefprojectionservice.h`, `sources/contactcrossrefprojectionservice.cpp`, `cmake/qet_compilation_vars.cmake`, `tests/qttest/tst_contactcrossrefprojectionservice.cpp`, and `tests/qttest/CMakeLists.txt`.
- Contact/CrossRef read-only projection prototype review: no blocking findings; API is narrow/read-only and leaves ownership, persistence, CLI/export, UI/rendering, `LinkElementCommand`, and Device/Function core untouched. CMake/testtarget weight remains the main follow-up risk.
- Contact/CrossRef negative projection test-only slice: `tests/qttest/tst_contactcrossrefprojectionservice.cpp` now creates temporary XML variants from `master_slave_links_group_index_minimal.qet` in `QTemporaryDir` and verifies missing `group_index`, out-of-range `group_index`, and slave/group contact type mismatch validation. No new fixture file was added.

## Known Issues
- A crash report from a Codex/ChatGPT launch context showed a very early Qt/Cocoa/AppKit startup abort before project, CAE, or PLC logic was reached. Treat this as a start-context/uninstalled-dev-bundle finding, not as evidence of a PLC/CAE defect.
- The project is not installed yet; any GUI/runtime conclusion requires a controlled separate Runtime-QA/installation check.
- Runtime-QA bundle inspection found an existing local build bundle at `build/baseline/qelectrotech.app` with `Contents/MacOS/qelectrotech` and `Contents/Info.plist`, but no embedded Qt frameworks/plugins under `Contents`; dependencies still resolve to Homebrew Qt paths such as `/opt/homebrew/opt/qtbase`, `/opt/homebrew/opt/qtsvg`, and `/opt/homebrew/opt/qtwebengine`. `codesign --display` reports an ad-hoc linker signature, while `codesign --verify --deep --strict` fails because the bundle has no resources despite the signature expecting them. Treat this as a dev/test bundle requiring packaging/deploy/signing work before any real installation claim.
- User-controlled manual launch of `build/baseline/qelectrotech.app` via `open -n` started successfully. Treat the earlier Codex/ChatGPT-context crash as a launch-context/dev-bundle finding, not as evidence of a PLC/CAE defect.
- `.gitignore` already has a local modification intentionally adding `.DS_Store` and `Handout.md`.
- `Handout.md` is a local unversioned project instruction file and must remain ignored/unversioned unless the user later explicitly decides to version a redacted/project-safe equivalent.
- CMake configure initialized/fetched project submodules as part of the existing project build flow.
- CMake reported optional scripting disabled: `Qt Qml module not available: JavaScript scripting (--run) disabled`.
- CMake reported missing Vulkan headers; this did not block configure/build.
- Build warnings observed in unchanged upstream source: self-assignment warning in `elementsmover.cpp`, ignored `nodiscard` result in `qet.cpp`, and an existing TODO pragma message in `openelmtcommand.cpp`.

## Next Planned Step
Visually verify coil mirrors for the rest of the Eaton contactor/relay combinations, especially changeover and multi-pole profiles, and check mirror spacing/position after moving and rotating coils in QET's GUI. Then implement side-mounted mirrors for motor-protection switches and fuses, followed by first-class early/late contact-function semantics. Separately wire the A3 sample frame into the MAM new-project default after GUI editing/printing has been checked.

## Change Log
- 2026-09-21: Created baseline progress record and documented repository/remotes.
- 2026-09-21: Recorded build blocker caused by missing active CMake/Ninja/Qt toolchain.
- 2026-09-21: Installed/provided Homebrew dependencies, configured and built unchanged baseline, ran tests, and verified non-interactive executable startup.
- 2026-09-21: Completed bounded read-only source survey and documented first-pass architecture findings in `Brain.md`.
- 2026-09-21: Completed read-only Phase 2 master/slave/contact mirror/cross-reference trace and documented findings in `Brain.md`.
- 2026-09-21: Completed read-only Phase 3 terminal/cable/potential trace and documented findings in `Brain.md`.
- 2026-09-21: Completed read-only Phase 4 numbering/autonum/conductor text trace and documented findings in `Brain.md`.
- 2026-09-21: Completed read-only Phase 5 XML/load-save compatibility trace and documented findings in `Brain.md`.
- 2026-09-21: Completed read-only Phase 6 regression-test map and documented coverage gaps and future test candidates in `Brain.md`.
- 2026-09-21: Completed read-only Phase 7 minimal fixture specification and documented reusable sources, planned fixture contents, assertions, risks, and abort criteria in `Brain.md`.
- 2026-09-21: Completed read-only Phase 8 expectation/normalization rules and documented robust comparison boundaries in `Brain.md`.
- 2026-09-21: Completed read-only Phase 9 prioritized non-implementing regression-test backlog and documented test names, hooks, helpers, fixtures, acceptance criteria, skip rules, effort, and risks in `Brain.md`.
- 2026-09-21: Completed read-only Phase 10 implementation sequence and review checklist for the first test-only infrastructure slice; no tests, fixtures, helpers, or source changes were created.
- 2026-09-21: Implemented first test-only patch `tst_cli_roundtrip_xml`, registered it in qttest CMake, built it, and verified it with targeted CTest runs.
- 2026-09-21: Reviewed first test-only patch against checklist, found no blocking findings, re-ran focused CTest verification, and documented the next P0 export-equivalence slice.
- 2026-09-21: Implemented P0 export-equivalence test-only slice, added minimal fixture, registered qttest target, built it, and verified new plus affected CLI tests.
- 2026-09-21: Reviewed P0 export-equivalence test-only slice against checklist, found no blocking findings, re-ran focused CTest verification, and documented next helper-extraction task.
- 2026-09-21: Extracted shared test-only CLI/CSV/JSON helpers into `tests/qttest/cli_test_utils.h`, updated both CLI tests, built them, and verified affected CLI tests.
- 2026-09-21: Reviewed helper extraction against checklist, found no blocking findings, re-ran focused CTest verification, and documented P0 options A/B with recommendation for Terminal/Potential export coverage first.
- 2026-09-21: Implemented P0 Terminal/Potential export coverage test-only slice, added minimal fixture, registered qttest target, built it, and verified new plus affected CLI tests.
- 2026-09-21: Implemented narrow Contact/CrossRef read-only projection prototype over loaded `QETProject`/`Element` state; added direct QtTest using `master_slave_links_group_index_minimal.qet`; rebuilt app target and verified 6 relevant tests.
- 2026-09-21: Reviewed Contact/CrossRef read-only projection prototype; re-ran focused build/CTest verification; documented CMake/testtarget-weight risk and next negative projection-test task.
- 2026-09-21: Implemented test-only negative Projection slice for missing/out-of-range `group_index` and contact type mismatch using temporary XML variants; re-ran focused build/CTest verification.

- 2026-09-21: Reviewed negative Contact/CrossRef projection slice; found no blocking issues, re-ran focused verification (6/6 relevant tests passed), and documented current project status and next decision options.
- 2026-09-21: Completed read-only CMake/Testtarget deweighting analysis for `tst_contactcrossrefprojectionservice`; created `Decision_Test_Target_Deweighting.md` with options, risks, acceptance criteria, abort rules, and recommendation.

- 2026-09-21: Implemented test-only CMake refactor for project-load qttests: introduced `add_qet_project_load_qtest` in `tests/qttest/CMakeLists.txt`, migrated only `tst_contactcrossrefprojectionservice`, kept assertions/fixture unchanged, and re-ran focused verification (6/6 relevant tests passed).

- 2026-09-21: Reviewed qttest project-load CMake refactor; found no blocking findings, confirmed assertions/fixture and forbidden areas unchanged, re-ran focused verification (6/6 relevant tests passed), and documented duplicate-assignment stop criteria.
- 2026-09-22: Implemented test-only Duplicate-assignment coverage in `tst_contactcrossrefprojectionservice` using a temporary XML variant from `master_slave_links_group_index_minimal.qet`; no API/service/fixture change; focused verification passed 6/6.
- 2026-09-22: Reviewed Duplicate-assignment coverage; found no blocking findings, confirmed scope remains Projection/Usage-vs-Capacity without Duplicate-Validation or PLC claims, and re-ran focused verification (6/6 relevant tests passed).
- 2026-09-22: Prepared review/commit-readiness report without staging/committing; documented working-tree status, proposed logical slices, unwanted build/.DS_Store artifacts, final focused test snapshot (6/6), and next cleanup/review actions.
- 2026-09-22: Cleaned local working-tree artifacts for commit preparation: added local `.git/info/exclude` rule for `build/`, removed `.DS_Store` files, and confirmed submodule dirtiness from Finder artifacts was cleared.
- 2026-09-22: Corrected documentation-slice stale-state findings in the read-only service spec, qttest deweighting decision note, and current next planned step; prepared for documentation-slice re-review.
- 2026-09-22: Reviewed code/test commit slice; found no blocking findings, confirmed fixtures are small and test-only, reran focused verification (6/6 relevant tests passed), and prepared suggested code/test commit split.
- 2026-09-22: Implemented Slice C P1 fix: added `Element::linkedElementsReadOnly() const` as a mutation-free link-list copy accessor, switched `ContactCrossRefProjectionService` away from mutating `linkedElements()`, and re-ran focused verification.
- 2026-09-22: Reviewed Slice C P1 API fix; focused tests passed 6/6, then corrected documentation history/current-status wording without code/test/fixture changes.
- 2026-09-22: Deduplicated byte-identical Slice B fixtures by renaming the shared export/terminal fixture to `workflow_exports_minimal.qet`, updating both CLI export tests, removing the duplicate test-only fixture, and re-running focused CLI tests.
- 2026-09-22: Decided Duplicate Assignment belongs in read-only Contact/CrossRef Projection validation; added duplicate group-assignment diagnostics to the service/API and focused QtTest coverage without UI, persistence, XML schema, or Device/Core changes.
- 2026-09-22: Prepared Duplicate Assignment validation projection as its own commit slice; roadmap remains PLC semantics analysis/spec next, with interactive GUI, full CTest, deeper PLC semantics, and default KF/ECM behavior still open.
- 2026-09-22: Completed PLC IO semantics analysis/spec slice in `Spec_PLC_IO_Semantics_ReadOnly.md`. Boundary: PLC truth remains `ElementData::PlcIO` plus master/slave `group_index`; slave `plc_*` fields are projection copies only; next implementation should be a warning-only `PlcIoProjectionService` extension.
- 2026-09-22: Implemented the smallest PLC IO read-only projection slice: normalized direction, terminal count/labels, and deterministic warning flags/messages for unlinked rows, out-of-range and duplicate `group_index`, empty addresses, and terminal-count/label mismatch; focused `tst_plcioprojectionservice` build/CTest passed.
- 2026-09-22: Documented that GUI remains unverified; the early Qt/Cocoa/AppKit crash report is a start-context/uninstalled-dev-bundle finding before project/CAE/PLC logic, and any runtime check belongs in a separate controlled Runtime-QA/installation slice.
- 2026-09-26: Completed read-only Runtime-QA bundle inspection for `build/baseline/qelectrotech.app`; found a dev bundle with executable and Info.plist only, external Homebrew Qt dependencies, ad-hoc linker signature, and failing strict codesign verification; no GUI launch, install, code change, test implementation, commit, reset, push, or `/Applications` mutation was performed.
- 2026-09-26: Recorded user-controlled manual `open -n build/baseline/qelectrotech.app` launch as successful; GUI startup is partially verified, the earlier Codex/ChatGPT crash remains a launch-context/dev-bundle finding, and clean install/deploy/codesign/packaging remain open.
- 2026-09-26: Re-audited Contact/CrossRef and PLC IO projection diagnostics; no service/test correction was needed, and the current scope remains read-only projection/validation with no UI, persistence, XML schema, Core, runtime, or packaging change.
- 2026-09-27: Added `mam_cae_library/`, an importable QET custom collection with 5 master coil patterns and 5 slave contact symbols. Hilfsschütz 40E/31E/22E contact layouts follow Eaton Schaltungsbuch 10/23, pp. 4-2–4-3; generic relay and power-contactor patterns are explicitly labeled as templates, not product articles. XML and contact-group counts validated; GUI import/rendering remains unverified because the dev executable could not acquire its SingleApplication shared-memory lock.
- 2026-09-27: Added separate coil/master definitions for all 24 standard contactor-relay combinations of DILA-40/31/22 with Eaton XHI02/11/20/04/13/22/31/40 blocks, using the resulting contact counts and combination codes from Eaton's catalog. Each completed combination is a separate Betriebsmittel; every auxiliary contact has its own contact group.
- 2026-09-27: Reshaped coil contact inventory so every auxiliary NO/NC contact is its own linkable group; changeover contacts are single 3-terminal groups, and the three main power poles are separate 1-pole groups. This follows the user's preference for separate components and keeps individual master/slave assignments available.
- 2026-09-27: Added manufacturer-neutral IEC master/slave symbols for early-make NO, late-make NO, late-break NC, and early-break NC contacts. Added six Eaton DILA-XHIV11/XHIV22 base-plus-block profiles with catalog codes and terminal labels. In current QET linking/rendering these special contacts are categorized in the ordinary NO/NC columns; first-class early/late semantics remain the next software slice. Siemens SIRIUS and Schneider TeSys references document the same general contact-function families.
- 2026-09-27: Reviewed Eaton Schaltungsbuch 10/23 printed pp. 4-43–4-44 visually. Page 4-44 shows PKZM/PKZ/PKE motor-protection devices with NHI11/AGM2 contacts to the right, using the same BMK `-Q1` as the base device. No fuse-with-auxiliary-contact schematic was located in the extracted book text; fuse contact/mapping remains user-directed scope, with exact labels/BMK to be checked against a direct example.

- 2026-09-27: Made coil contact mirrors independent of saved cross-reference display preferences; coils with declared groups now show all slots below the coil, while linked slave slots retain coordinate references. Added an A3 QET example and visually checked linked and unlinked contact groups through headless PNG export. Interactive GUI verification remains open.

### 2026-09-27 update - A3-Schriftfeld nach Nutzerreferenz korrigiert

- Der bisherige gleichmäßige Tabellenaufbau entsprach nicht dem gezeigten Schriftfeld und wurde ersetzt: jetzt gestufte Inhaltsfelder mit ungleichen Spalten, separatem Logo-/Firmenplatz, Auftraggeber-/Projektkennung sowie Revisions-, Ersteller-, Freigabe-, Format-, Blatt- und Seitenfeldern.
- QET-Vorlage akzeptiert für Rasterbreiten nur ganze Prozentwerte. Das neue Raster nutzt deshalb ganzzahlige Breiten; die Vorlage und beide eingebetteten Projektkopien sind XML-geprüft und im Headless-Export visuell geprüft.
- Alle Nutzinhalte sind als Schriftfeldwerte/Variablen angelegt. Ohne Eingabe zeigt QET benutzerdefinierte Variablen sichtbar als `%{…}` an; die Funktionsprobe demonstriert daher zugleich, welche Felder noch Projektwerte benötigen. Ein herstellerspezifisches Logo wurde nicht übernommen.
- Die Vorlagenseite listet mehrere Dokumentarten (u. a. Klemmenplan, SPS-Übersicht, Kabelübersicht und Montageplatte). Aktuell umgesetzt ist ausschließlich der gemeinsame A3-Grundrahmen mit dem hier gezeigten Schriftfeld; eigene Auswertungs-/Dokumentvorlagen für diese Dokumentarten sind noch offen.

### 2026-09-27 update - unteres Schriftfeld anhand vollständiger Ausschnitte rekonstruiert

- Ersetzt den vorherigen Entwurf durch die Struktur aus den drei nachgereichten Referenzbildern: drei Revisions-/Änderungsspalten, dreizeilige Datum/Bearbeiter/Geprüft-Tabelle, Projektname, Firmenlogo-Platz, Dokumenttitel sowie untere Kennzeichnungszeile mit Norm, Ursprung, Ersetzt-für/Ersetzt-durch, Dokumentnummer und Strukturangaben.
- Die Feldinhalte bleiben als QET-Variablen editierbar. Die Vorlagenkopie in beiden MAM-A3-Beispielen wurde synchronisiert. XML-Prüfung und Headless-PDF-Export funktionieren; `pdfinfo` bestätigt A3.
- Referenz zeigt den Zonenstreifen mit Spaltenziffern unter dem Schriftfeld, während QET seinen Spaltenkopf derzeit oberhalb der Zeichnungsfläche rendert. Diese Lage kann mit dem gegenwärtigen Rahmenmodell nicht per Schriftfeldvorlage verschoben werden. Die Vorlage reproduziert daher den Schriftfeldabschnitt, aber noch nicht die komplette Rand-/Zonenaufteilung des EPLAN-Blatts.

### 2026-09-27 correction - actual A3 sheet frame, not just the title block

- The frame model now supports a repeated column-number strip below rows A-F, row labels on both left and right, and an optional outer sheet rectangle. These are stored per diagram in XML, preserved in border settings, and editable in the border properties panel. Existing QET sheets keep the prior layout unless the new options are enabled.
- The prototype examples enabled a layout that included top and bottom 0-9 column bands, six A-F row bands at both edges, an outer frame, and the then reconstructed title block. The user later rejected the implementation against `Rahmenvorlage.png`; the headless export below is historical structural evidence only, not acceptance.
- Updated the contact mirror example positions to retain the intended B6/B7 cross-references after the grid dimensions changed.
- Build succeeded. Headless PNG/PDF export succeeded; the visual export shows both column bands, A-F on both sides, the outer frame and title block. `pdfinfo` reports one A3 landscape page (1191 x 843 pt). XML parsing and `git diff --check` pass.

### 2026-09-28 - Rahmenstatus und Bestandsaufnahme vor Korrektur

- Die letzte Rahmenumsetzung wurde vom Nutzer ausdrücklich zurückgewiesen. Sie ist nicht abgenommen; frühere Aussagen über eine visuelle Prüfung sind keine Akzeptanz.
- Verbindliche Vergleichsquelle ist ausschließlich die projektlokale `Rahmenvorlage.png`. Der Abgleich muss das ganze A3-Blatt umfassen: äußeren Rand, Koordinatenstreifen, gesamtes Zeichenfeld, Seitenverweise und vollständiges Schriftfeld.
- Der dokumentierte macOS-26.6.1/Cocoa/Qt-Absturz (`EXC_CRASH`, `SIGABRT` in `HIServices::_RegisterApplication` über AppKit und `libqcocoa`) bleibt aktiver Blocker für GUI-Verifikation.
- Ein Headless-PNG/PDF-Export ist nur Strukturprüfung und ersetzt weder GUI-Öffnen noch visuelle Prüfung im QET-Editor oder in der Druckvorschau.
- Beim aktuellen Bestandsaufnahmeversuch ließ sich mit `build/baseline/qelectrotech.app/Contents/MacOS/qelectrotech --export-png ...` kein neuer Export erzeugen: der Prozess endete mit Exit-Code 134 und Cocoa/HIServices-Verbindungsfehlern. Der Vergleich aus `/private/tmp/mam-current.png/01_Klemmenplan.png` ist ein vorhandener älterer Export und daher nur vorläufige Strukturevidenz.
- Sichtbarer Ganzblattvergleich dieses vorhandenen Exports mit `Rahmenvorlage.png`: zusätzliche Überschrift „Klemmenplan“, seitliche A-F-Zonen, abgeschnittene Ecken und abweichende Aufteilung/Proportionen des Schriftfelds. Die Referenz hat einen oberen 0-9-Streifen, ein freies großes Zeichenfeld, Seitenverweise `1`/`4` oberhalb des Schriftfelds und keine A-F-Streifen. Abweichungen bleiben offen; keine Teilfläche gilt als akzeptiert.
- Für den nächsten Korrekturschritt sind zuerst `examples/MAM_EPLAN_A3_Klemmenplan.qet` (Blattrahmenattribute und eingebettete Vorlage) sowie `titleblocks/MAM_EPLAN_Klemmenplan_A3.titleblock` (separate Vorlagenkopie) gegeneinander zu prüfen und konsistent zu bearbeiten. Die relevanten Render-/Geometriepfade liegen in `sources/bordertitleblock.cpp` und `.h`; gespeicherte Rahmeneigenschaften in `sources/borderproperties.cpp` und `.h`; UI-Editierbarkeit in `sources/ui/borderpropertieswidget.cpp` und `.ui`; Schriftfeldzellen im `sources/titleblocktemplate.cpp` sowie `sources/titleblockcell.cpp` und `.h`. `sources/diagram.cpp` ist zusätzlich zu prüfen, falls Zeichenflächenbegrenzung oder Szenegeometrie den Ganzblattaufbau beeinflusst.
- Die vorhandenen dirty Änderungen dieser Quelldateien und Beispiel-/Vorlagendateien stammen aus dem bestehenden Arbeitsbaum. Vor einer Codeänderung muss geklärt werden, ob die Abweichung durch Projekt-/Vorlagenwerte oder durch eine Renderbegrenzung entsteht; keine breite Rücknahme oder Bereinigung.

### 2026-09-28 - Headless-Export mit Offscreen-Qt erfolgreich

- Ursache des vorherigen Exportabbruchs eingegrenzt: ohne Plattformvorgabe startete Qt auf macOS über Cocoa/AppKit und endete mit Exit-Code 134 sowie Pasteboard-/HIServices-Verbindungsfehlern. Mit `QT_QPA_PLATFORM=offscreen` wird der Export ohne Cocoa-GUI-Backend ausgeführt und gelang erfolgreich.
- Befehl: `QT_QPA_PLATFORM=offscreen build/baseline/qelectrotech.app/Contents/MacOS/qelectrotech --export-png examples/MAM_EPLAN_A3_Klemmenplan.qet /private/tmp/frame-audit-offscreen.png`.
- Frischer Export: `/private/tmp/frame-audit-offscreen.png/01_Durchlaufofen.png`, 1518 × 1075 px. Das ist Headless-Struktur-/Darstellungsevidenz, keine GUI-Verifikation und keine Freigabe des Rahmens.
- Ganzblattvergleich mit `Rahmenvorlage.png`: Top-0-9-Streifen und großes freies Zeichenfeld sind vorhanden; im frischen Bild sind die zuvor im älteren PNG sichtbare Zusatzüberschrift, A-F-Seitenstreifen und diagonalen Ecken nicht vorhanden. Weiterhin weichen die Gesamtproportionen und innere Feldaufteilung des unteren Schriftfeldes ab; der sichtbare Firmenlogo-Platz bleibt im Beispiel leer bzw. ohne eingebettete Grafik, die Seitenverweise `1`/`4` aus der Referenz sind nicht sichtbar. Rahmen bleibt zurückgewiesen und offen.
- Keine Layout-/Rahmen- oder Titleblock-Geometrie geändert. Die vorangehende Dokumentation, dass der Standard-Cocoa-Aufruf fehlschlug, bleibt als Befund korrekt; `QT_QPA_PLATFORM=offscreen` ist die lokale Exportumgehung.

### 2026-09-28 - Erste gezielte Schriftfeldkorrektur
- Vor dem Eingriff `Rahmenvorlage.png` (4532 × 3210 px) und frischen Offscreen-Export (1518 × 1075 px) ganzblattbezogen geprüft. Gemessene Orientierungspunkte: oberer 0–9-Streifen y≈8–64 px; Schriftfeldoberkante y≈2936–2940 px und Unterkante y≈3193 px; vertikale Hauptteilungen etwa x≈1713, 2367, 3226 und 3902 px. Diese Quellbild-Pixelwerte dienen als Proportionen, nicht als QET-Maße.
- Schriftfeld in Projekt- und separater Vorlagenkopie neu proportioniert, Referenzlogo aus der Vorlage eingebettet, Beispielwerte ergänzt und Seitenverweise oberhalb des Schriftfelds an die Folionachbarn gebunden. Das Beispieldiagramm wurde zur passenden A3-Proportion angepasst.
- Geänderte Rahmen-/Vorlagendateien: `sources/bordertitleblock.cpp`, `titleblocks/MAM_EPLAN_Klemmenplan_A3.titleblock`, `examples/MAM_EPLAN_A3_Klemmenplan.qet`. Nachweise: Build `cmake --build build/baseline --target qelectrotech -j 4`; frischer Export `/private/tmp/frame-audit-final.png/01_Durchlaufofen.png` (1518 × 1075); synthetischer Dreiseitentest `/private/tmp/frame-audit-neighbor-pages.png/02_Durchlaufofen.png` zeigt Verweise 1 und 4. XML-Parse und `git diff --check` erfolgreich.
- Näher an der Vorlage liegen nun Schriftfeld-Höhe und Hauptspalten-Proportionen, Logo, Reserveseitenfeld sowie benachbarte Blattverweise. Offen bleiben feine interne Linien-/Textpositionen und Schriftgrößen; das Einseitenbeispiel zeigt keine Nachbarverweise. Es gab keine GUI-Prüfung: der dokumentierte macOS/Cocoa/Qt-Crash bleibt aktiver Blocker. Headless-Export ist nur Strukturprüfung; Rahmen nicht abgenommen.

### 2026-09-28 - QET-Cocoa-Startcrash erneut eingegrenzt
- Den vom Nutzer neu eingebrachten vollständigen Crashbericht `/Users/michaelmartin/.codex/attachments/83bdde15-2711-4494-9d2b-013625b8d2ee/Eingefügter Text.txt` gelesen und mit den lokalen Reports `qelectrotech-2026-09-28-021019.ips`, `023211.ips`, `025334.ips` und `025339.ips` verglichen. Der neue Report ist der frische CLI-Probecrash vom 03:28:15, PID 52481, Elternprozess Python, verantwortlich ChatGPT/Codex-Koalition.
- Reproduktionsbefehl (nichtinteraktiver Export, kein Projekt-Schreibzugriff): `env -u QT_QPA_PLATFORM build/baseline/qelectrotech.app/Contents/MacOS/qelectrotech --export-png examples/MAM_EPLAN_A3_Klemmenplan.qet /private/tmp/qet-cocoa-start-probe.png`; Exitcode als Signal 6 (`-6`). Log: `/private/tmp/qet-cocoa-start-probe.log`. Ausgabe: Pasteboard/HIServices-XPC-Fehler. Der Crash tritt schon beim Erzeugen von `QApplication` auf, bevor QET das Projekt verarbeitet.
- Der vollständige Stack bleibt identisch: `HIServices::_RegisterApplication` → AppKit `NSApplication` → Homebrew-Qt `libqcocoa` → Qt 6.11.2 `QApplication`. Im aktuellen Pfad `sources/main.cpp` geschieht dies vor GUI-/Projektlogik; der Headless-Exportpfad erstellt ebenfalls `QApplication`, weshalb er ohne `QT_QPA_PLATFORM=offscreen` auf demselben Backendcrash endet.
- Bundle-Befund: `build/baseline/qelectrotech.app` enthält nur `Contents/MacOS` und `Info.plist`; Qt-Frameworks/Plugins sind nicht eingebettet und werden aus `/opt/homebrew` geladen. Die Bundle-ID ist gesetzt (`org.qelectrotech.QElectroTech`), und die geladene Cocoa-Plugin-/Qt-Familie ist durchgehend 6.11.2. Das unvollständige Dev-Bundle ist ein Deployment-Risiko, aber aus dem Stack nicht als direkte Crashursache bewiesen. Historisch dokumentierter, vom Nutzer kontrollierter Start per `open -n` war erfolgreich; dieser Schritt hat keinen GUI-Start ausgelöst.
- Kein gezielter QET-Codefehler und kein belastbarer sicherer Quellcodefix wurden gefunden; keine Repo-Quelldatei geändert. GUI-Verifikation bleibt bis zu einem kontrollierten Start der aktuellen Bundle-Version über LaunchServices beziehungsweise einem vollständigen Deploy-Bundle und erfolgreichem Sichttest offen. Nichtinteraktiver Offscreen-Export bleibt nur Strukturprüfung.

### 2026-09-28 - LaunchServices-Starttest aus Codex-Umgebung
- Nichtinteraktiver LaunchServices-Test mit `open -n -W -a <bundle> --args --export-png <projekt> <tmp-png>` und auch mit direktem Bundlepfad schlug vor dem QET-Prozess fehl: `NSOSStatusErrorDomain Code=-10827`, `kLSNoExecutableErr`. Kein Exportartefakt entstand, und nach dem Versuch wurde kein neuer QET-`.ips`-Crashreport geschrieben. Das prüft den Cocoa-Crash daher nicht erfolgreich; LaunchServices startete die App in dieser Umgebung gar nicht.
- Das originale `build/baseline/qelectrotech.app` hat laut `ls`, `file` und `PlistBuddy` eine vorhandene ausführbare ARM64-Datei `Contents/MacOS/qelectrotech` und `CFBundleExecutable=qelectrotech`; `Info.plist`/Bundle-ID sind vorhanden. Das Bundle enthält zunächst weder `Contents/Resources` noch Qt-Frameworks/Plugins. Prozesslistenprüfung war im Toolrunner eingeschränkt (`ps`: operation not permitted; `pgrep`: sysmond service not found); nach dem expliziten LS-Fehler ist kein gestarteter QET-Prozess zu erwarten, aber eine systemweite PID-Liste war nicht verfügbar.
- Eingrenzung in einer Kopie außerhalb des Repos: `ditto` nach `/private/tmp/qet-launchservices-test.app`, minimale `Contents/Resources` und `PkgInfo` ergänzt, ad-hoc signiert und `codesign --verify --deep --strict` als gültig bestätigt. LaunchServices gab trotzdem denselben `kLSNoExecutableErr` zurück. Eine zweite Kopie `/private/tmp/qet-macdeploy-launchservices.app` erhielt `macdeployqt`-Frameworks und -Plugins, wurde von `codesign --verify` aber als ungültig signiert gemeldet; LaunchServices lieferte ebenfalls `-10827`. Diese Versuche verändern keine Repo-Dateien.
- Ergebnis: Ein LaunchServices-Start aus der Codex-Toolumgebung umgeht den Crash nicht nachweislich, weil die App nicht gestartet wird. Kein Quellcode-Fix und keine Rahmenänderung. Praktischer nächster Schritt ist ein Start des korrekt deployten/signierten Bundles direkt durch den Nutzer über Finder oder ein reguläres Terminal außerhalb des Codex-Toolrunners; dort die tatsächliche Fensteröffnung bestätigen. Historischer `open -n`-Erfolg ist dokumentiert, ersetzt diesen aktuellen Nachweis nicht. GUI-Verifikation bleibt blockiert.

### 2026-09-28 - Nutzer bestätigt QET-Start außerhalb Codex
- Neue Nutzerrückmeldung: QET startet erfolgreich, wenn es außerhalb von Codex über den normalen macOS-Startweg geöffnet wird. Die bisherigen Cocoa/HIServices-Abbrüche beim direkten CLI-/Codex-Kindprozess und `kLSNoExecutableErr` aus dem Codex-Toolrunner bleiben für diese Startkontexte dokumentiert; sie widerlegen den erfolgreichen Nutzerstart nicht.
- Der breite Startblocker für GUI-Prüfung ist damit laut Nutzerbericht aufgehoben: eine native GUI-Verifikation ist grundsätzlich wieder möglich. Die konkrete visuelle Prüfung des aktuellen A3-Rahmens gegen `Rahmenvorlage.png` steht weiterhin aus. Der zurückgewiesene Rahmenstand gilt ausdrücklich nicht als akzeptiert. Headless-Export bleibt auf Strukturprüfung beschränkt.
- Bei der nicht-destruktiven Statusprüfung wurden keine neuen Änderungen an Rahmengeometrie oder Quelldateien aus dem unterbrochenen Nachschärfschritt festgestellt. Die bereits dirty Änderungen im Beispiel, in Rahmen-/QET-Quelldateien und in der CAE-Bibliothek bestanden im Arbeitsbaum und wurden nicht zurückgesetzt oder bereinigt.

### 2026-09-28 - A3-Rahmen: Ganzblatt-Abgleich und vertikale Proportion
- Vor der Änderung Vorlage und frischen Offscreen-Export ganzblattbezogen verglichen. Vorlage: 4532 × 3210 px; Export vorher: 1518 × 1075 px. Auf gemeinsame Breite skaliert liegen Zahlenstreifen (0–9, zehn Zellen), Seitenbreite und Hauptfeld fast deckungsgleich. Die erkennbaren Hauptteilungen des Schriftfelds lagen im Export nur wenige Pixel neben der Vorlage. Deutlich messbar war die Vertikalposition: Referenz-Schriftfeldoberkante y≈983 px, Unterkante y≈1069 px; vorheriger Export y≈989 bis 1073 px. Vorherige relative mittlere absolute Pixelabweichung nach Referenzskalierung: 0,01623.
- Gezielte Änderung: sechs Schriftfeldzeilen auf insgesamt 86 px Renderhöhe verteilt (vorher 84); das Zeichenfeld um 6 QET-Einheiten verkürzt, damit die Schriftfeldoberkante hochrückt. Die Zeilenverteilung und Diagrammzeilenhöhe sind im Beispiel mit eingebetteter Vorlage konsistent gehalten; die externe Titelblockvorlage wurde ebenfalls aktualisiert.
- Geänderte Dateien: `examples/MAM_EPLAN_A3_Klemmenplan.qet` und `titleblocks/MAM_EPLAN_Klemmenplan_A3.titleblock`. Keine Änderung an `sources/` oder sonstigen Dateien in diesem Schritt.
- Verifikation: `cmake --build build/baseline --target qelectrotech -j 4` (keine Neukompilierung nötig); frischer Export mit `QT_QPA_PLATFORM=offscreen build/baseline/qelectrotech.app/Contents/MacOS/qelectrotech --export-png examples/MAM_EPLAN_A3_Klemmenplan.qet /private/tmp/frame-audit-corrected.png`; Ergebnis `/private/tmp/frame-audit-corrected.png/01_Durchlaufofen.png` (1518 × 1071 px). Für den pixelweisen Ganzblattvergleich auf 1518 × 1075 gepolstert: `/private/tmp/frame-current-padded.png`; skaliertes Referenzbild: `/private/tmp/frame-reference-scaled.png`; Überlagerung: `/private/tmp/frame-audit-overlay-corrected.png`; Differenzbild: `/private/tmp/frame-audit-diff-padded.png`. Mittlere absolute Abweichung fiel auf 0,01258 (ca. 22 % niedriger); der Footer sitzt nun ungefähr bei y=983–1069 px.
- Sichtbar näher: obere Schriftfeldkante, Gesamthöhe des Schriftfelds und dessen Unterkante. Zahlenstreifen, große Zeichenfläche, Logo- und Hauptspaltenpositionen waren bereits nahe und blieben unangetastet. Weiter offen sind Zelltext-/Schriftunterschiede, Unterschiede durch Beispielwerte (z. B. Blattangabe) und die endgültige native QET-GUI-Abnahme. Der einzelne Export enthält zwar die dynamischen Randverweise 1/4, jedoch kann er die Vorlage nicht inhaltlich vollständig spiegeln.
- GUI-Abnahme braucht noch eine Sichtprüfung des Nutzers im normal gestarteten QET: `examples/MAM_EPLAN_A3_Klemmenplan.qet` öffnen und über die gesamte Seite mit `Rahmenvorlage.png` vergleichen. Der Headless-Export ist nur Struktur-/Bildbeleg, keine Annahme des weiterhin zurückgewiesenen Rahmenstands.

### 2026-09-28 - GUI-Screenshot und Codepfade des Rahmens
- Den Nutzer-Screenshot als visuelle Evidenz (links QET, rechts Vorlage) mit `Rahmenvorlage.png` und frischem ganzem Offscreen-Export verglichen. Konkreter Fehler unten links: `approval-title` und `software-version` forderten `colspan="6"`, während die Logo-Zelle in Spalte 6 liegt. Laut `TitleBlockTemplate::checkCellSpan` sind span-Werte zusätzliche Zellen; die Kollision deaktiviert daher den Span und quetscht diese Texte in die erste 5%-Spalte. Für die sechs gewünschten Spalten lautet der Wert 5. Beide Felder sind nun mit `colspan="5"` gesetzt; Softwaretext ist rechtsbündig, Schriftgrad 6 und die Projekteigenschaft enthält nur `2024.0.3 Build: 21729`, da das Feldlabel `EPLAN P8 Version:` bereits im Template steht. Kunden-/Projektlabel wurden entsprechend der Referenz ohne Schluss-Doppelpunkt gesetzt.
- Codepfade geprüft: `Diagram::readXml` lädt die Border-Attribute aus dem Diagramm-XML und `BorderTitleBlock::borderFromXml` baut daraus die Zeichenfläche (hier `cols=10`, `colsize=151.15`, `colheaderheight=23.4`, `rows=6`, `rowsize=160.033333`). Feldmaße, Inhalte, Logo und Zeilenhöhen stammen aus der im Beispiel eingebetteten `TitleBlockTemplate`-Griddefinition; die separate `.titleblock`-Datei ist die wiederverwendbare Vorlagenkopie. `BorderProperties`-Konstruktorwerte 17/60/20 sind bei den expliziten Projektwerten nicht wirksam. Kein belegter Rendererfehler; die falsch geforderten XML-Spans waren die Ursache, daher kein C++-Layoutumbau.
- Separater codegesteuerter Rahmenwert: Spaltennummerierung kommt aus globalem `QSettings`-Key `border-columns_0` in `BorderTitleBlock::draw`. Der frische Export zeigt 1–10, während die Vorlage 0–9 zeigt; die aktive Umgebung liefert offenbar false. `GeneralConfigurationPage` verwendet als UI-Fallback ebenfalls false, aber Zeichnung/Positionspfad nutzen true als Fallback. Das ist eine Default-Inkonsistenz; ein gespeicherter Nutzerwert übersteuert beide Defaults. Es wurde keine globale Nutzereinstellung verändert. Für 0–9 muss die globale Einstellung in QET eingeschaltet sein; diese projektexterne Kopplung bleibt eine offene Reproduzierbarkeitsgrenze.
- Im Screenshot waren Hauptteilungen der Felder für Logo, Reserve und rechte Informationsgruppe proportional bereits nah. Nach Korrektur spannt der Genehmigungstitel über die gesamte linke Gruppe und der Versionshinweis sitzt rechts am unteren Rand dieser Gruppe statt in der ersten schmalen Spalte. Kunden-/Projektlabels entsprechen nun dem sichtbaren Referenztext. Projektabhängige Blattangaben (Beispiel Blatt 1, Vorlage Blatt 2) unterscheiden sich weiterhin erwartungsgemäß.
- Geänderte Dateien dieses Schritts: `examples/MAM_EPLAN_A3_Klemmenplan.qet` und `titleblocks/MAM_EPLAN_Klemmenplan_A3.titleblock`; keine C++-Datei geändert. Der vorige Schritt hatte dieselben beiden Vorlagen zusätzlich an Schriftfeldhöhe/Diagrammzeilenhöhe, Softwaretext und Labeltext angepasst; diese Änderungen wurden beibehalten, nichts zurückgesetzt.
- Build: `cmake --build build/baseline --target qelectrotech -j 4` (keine Neukompilierung nötig). Frischer Offscreen-Export: `/private/tmp/frame-audit-code-analysis-final.png/01_Durchlaufofen.png`, 1518 × 1071 px. Ganzblatt-Skalierung/Überlagerung: `/private/tmp/frame-ref-current.png`, `/private/tmp/frame-audit-overlay-final.png`; Für MAE-Vergleich wurde der 1518 × 1071-Export unten weiß auf 1518 × 1075 ergänzt: `/private/tmp/frame-audit-final-padded.png`; Differenzbild `/private/tmp/frame-audit-final-padded-diff.png`, MAE 0,0125968. Detail des linken Softwarefelds `/private/tmp/frame-audit-software-final.png`. `git diff --check` sauber. Dies bleibt Headless-Strukturbeleg.
- Wegen Änderungen nach dem gelieferten GUI-Screenshot ist ein neuer GUI-Screenshot für Abnahme erforderlich. Rahmen bleibt nicht angenommen; bitte im normal gestarteten QET die aktualisierte Beispielseite ganzblattbezogen gegen `Rahmenvorlage.png` prüfen.

### 2026-09-28 - A3-Spaltenzählung projektbezogen auf 0–9 festgelegt
- Ursache bestätigt: `BorderTitleBlock::draw` las `border-columns_0` ausschließlich aus globalem `QSettings`; das Projekt konnte die Nummerierung nicht selbst festlegen. Der Offscreen-Export vor der Änderung zeigte trotz Zielvorlage 0–9 die Spalten 1–10. Die globale Einstellung wurde nicht verändert.
- Eng begrenzte Projektoption `columnstartatzero` ergänzt. Sie wird in `BorderProperties` geladen/gespeichert und in den Diagramm-Rahmeneigenschaften über „Spalten bei 0 beginnen“ bearbeitet. `BorderTitleBlock` nutzt sie für oberen/unteren Nummerierungsstreifen, DXF und Zellpositionen; `CellRuler` und `DiagramPosition` bleiben konsistent. Alte Projektdateien ohne Attribut übernehmen weiterhin den bisherigen globalen `border-columns_0`-Wert. Das A3-Beispiel setzt die neue Option explizit auf `true`.
- Build erfolgreich: `cmake --build build/baseline --target qelectrotech -j 4` (exit 0; ein vorhandener Compilerhinweis in `sources/elementsmover.cpp` zu Selbstzuweisung blieb unverändert). Frischer Offscreen-Export `/private/tmp/frame-zero-audit-after.png/01_Durchlaufofen.png` (1518 × 1071); der obere Streifen zeigt nachweislich `0,1,2,3,4,5,6,7,8,9`. Detail `/private/tmp/frame-zero-audit-top.png`. `git diff --check` sauber.
- Geändert in diesem Schritt: `sources/borderproperties.h/.cpp`, `sources/bordertitleblock.h/.cpp`, `sources/cellruler.cpp`, `sources/diagramposition.h/.cpp`, `sources/ui/borderpropertieswidget.ui/.cpp`, `examples/MAM_EPLAN_A3_Klemmenplan.qet`, `Progress.md`, `Brain.md`. Keine Nutzereinstellung angepasst. GUI-Abnahme des aktualisierten ganzen A3-Blatts bleibt offen; Offscreen ist nur Strukturprüfung.

### 2026-09-28 - Leeres Feld vor der A3-Kopfnummerierung entfernt
- Nutzer-Screenshot und Vorlagenvergleich zeigten vor der `0` eine leere Zelle. Ursache im Renderer: `BorderTitleBlock` reservierte und zeichnete die linke Zeilenkopfbreite unabhängig davon, ob Zeilenköpfe sichtbar waren (`displayrows="false"` im Beispiel). `columnstartatzero` setzte zwar die Ziffern auf 0–9, ließ diesen reservierten Kopfbereich aber bestehen.
- Geändert: `sources/bordertitleblock.cpp/.h` berücksichtigen die Zeilenkopfbreite nur bei sichtbaren Zeilenköpfen, einschließlich Diagrammbreite, Spaltenrechtecken und PNG/DXF-Zeichenpfad. `sources/cellruler.cpp` verwendet denselben Offset.
- Build erfolgreich: `cmake --build build/baseline --target qelectrotech -j 4`. Frischer Offscreen-Export `/private/tmp/frame-zero-leftfix.png/01_Durchlaufofen.png` (1513 × 1071); Detail `/private/tmp/frame-zero-leftfix-top-left.png` zeigt `0` direkt in der ersten linken Kopfzelle, ohne leere Zelle davor. Referenzdetail: `/private/tmp/frame-zero-reference-top-left.png`. `git diff --check` sauber.
- Änderung verringert die Exportbreite gegenüber vorher von 1518 auf 1513 px, da der nicht angezeigte Zeilenkopfbereich entfällt. GUI-Abnahme mit aktuellem Screenshot bleibt erforderlich; Headless-Export ist nur Strukturbeleg.

### 2026-09-28 - GUI zeigt trotz Projektoption weiterhin 1–10
- Neuer GUI-Screenshot `/var/folders/j7/g4r4216905q3l3g35ndgk_rm0000gn/T/codex-clipboard-a9603dad-3cab-4ce3-b4c7-d6bb2afdb13d.png` zeigt weiterhin `1–10` und links davor den schmalen leeren Streifen. Das entspricht dem alten Verhalten vor den zuletzt gebauten Änderungen.
- Nicht-destruktive Prüfung: `examples/MAM_EPLAN_A3_Klemmenplan.qet` enthält `columnstartatzero="true"`; `BorderTitleBlock::borderFromXml` liest diese Option beim Diagramm-Laden. Gebautes Binary ist neuer als die geänderten Dateien (Binary 04:41:06, Renderer-Quellen 04:39:36/04:39:56), Hash `168605fc61a105a01e3b1e742b78c698ebffb8d3c665fa913474f80dae4c50e9`. `defaults read org.qelectrotech.QElectroTech border-columns_0` ergibt `0` (false), hat aber für dieses Projekt nur Bedeutung, falls die XML-Option nicht geladen wird.
- Frischer Export mit genau diesem Binary und genau dieser absoluten Projektdatei, `QT_QPA_PLATFORM=offscreen build/baseline/qelectrotech.app/Contents/MacOS/qelectrotech --export-png examples/MAM_EPLAN_A3_Klemmenplan.qet /private/tmp/gui-numbering-check.png`, öffnete das 63-KiB-Projekt und ergab `/private/tmp/gui-numbering-check.png/01_Durchlaufofen.png` (1513 × 1071). Sichtprüfung dieses Exports zeigt ohne Leerzelle `0–9`. Aktueller Screenshot und Export widersprechen sich daher tatsächlich; Export allein belegt nicht, was in der GUI läuft.
- Wahrscheinlichster Befund ist ein bereits offenes/älteres QET-Bundle oder ein im alten Prozess geladener Projektzustand. Es wurde kein Code oder Projektrahmen geändert. GUI-Prüfung: QET vollständig mit `⌘Q` beenden; im Finder „Gehe zum Ordner …“ öffnen und `/Volumes/MAM Home/Github/QElectrotech_MAM/build/baseline/qelectrotech.app` starten; danach in genau diesem QET über „Datei > Öffnen“ `/Volumes/MAM Home/Github/QElectrotech_MAM/examples/MAM_EPLAN_A3_Klemmenplan.qet` öffnen. Falls Finder das Build-Bundle nicht starten kann, nicht den alten GUI-Screenshot als Prüfung des neuen Stands werten; den Startfehler melden. Kein GUI-Prompt wurde bedient.

### 2026-09-28 - MAM-Build hart auf Deutsch festgelegt
- QET hatte `QSettings("lang")` verwendet und bei `system`/leer die macOS-`QLocale::system()`-Sprache übernommen. Der aktuelle Host meldet inzwischen `AppleLanguages=de-DE` und die gemeinsame QET-Präferenz `lang=de`; der vom Nutzer berichtete französische GUI-Stand lässt sich daher nicht der aktuell gelesenen Präferenz belegen und kann zu einem anderen/älteren Bundle gehören. Die GUI-Screenshot-Aufnahme zeigt nicht die Menüs.
- Der MAM-Build ignoriert jetzt diese beiden Quellen: `QETApp::langFromSetting()` liefert fest `de`. Die Sprach-Auswahl zeigt „Deutsch“ deaktiviert; allgemeines Speichern der Einstellungen schreibt die gemeinsame QET-Sprachpräferenz nicht mehr um. Das deutsche QET-Katalogfile ist nicht leer (3.025/3.026 Einträge befüllt).
- Die macOS-Build-App hatte noch kein `Contents/Resources/lang`; QETs kompilierter Bundle-Pfad `../Resources/lang/` konnte dadurch den deutschen Katalog nicht finden. CMake kopiert `qet_de.qm` nach dem Linken in diesen Bundle-Pfad.
- Verifikation: `cmake --build build/baseline --target qelectrotech -j 4` erfolgreich; die passende Translation liegt unter `build/baseline/qelectrotech.app/Contents/Resources/lang/qet_de.qm`. Finaler Offscreen-Projekt-Export mit demselben Binary ergab `/private/tmp/mam-a3-final-export.png/01_Durchlaufofen.png` (1513 × 1071); `git diff --check` sauber. Kein GUI-Test; Sprache nach vollständigem Neustart des neu gebauten Bundles visuell bestätigen.
- Sprachänderung in `sources/qetapp.cpp`, `sources/ui/configpage/generalconfigurationpage.cpp` und `CMakeLists.txt`. In denselben beiden C++-Dateien bestehen zusätzlich ältere, separate Schriftartänderungen; diese werden bei der Commit-Auswahl nicht mitgestaged.
- Windows-Nachtrag: `CMakeLists.txt` hängt den QET-Zielbuild jetzt explizit an `${PROJECT_NAME}_lrelease` und legt bei Nicht-macOS-Builds `qet_de.qm` unter `lang/` neben die EXE. Damit startet auch `build/windows-msvc-app/qelectrotech.exe` per Doppelklick mit verfügbarem deutschen QET-Katalog, ohne `--lang-dir` oder Installationsschritt. Verifikation im Windows-MSVC-Build: `build/windows-msvc-app/lang/qet_de.qm` vorhanden (392827 Bytes), Rebuild erfolgreich, `qelectrotech.exe --version` meldet `0.200.1-dev`, `git diff --check` sauber.

### 2026-09-28 - MAM-Blattrand auf echten Seitenursprung gesetzt
- Produktentscheidung: Es gibt keine alten MAM-Projekte, die historische QET-Seitenränder schützen müssen. Ziel ist ein produktiver MAM-CAE-Fork mit EPLAN/WSCAD-ähnlichem Arbeitsblattverhalten, daher darf die Blattgeometrie von QET-Defaults abweichen.
- Das A3-Beispiel hatte bereits `outerbordermargin="0"`. Der verbleibende sichtbare Ursprungsoffset kam aus dem globalen `Diagram::margin = 5.0`, den `BorderTitleBlock`, Zelllineal und Exportpfade als linken/oberen Seitenursprung verwenden.
- Geändert: `sources/diagram.cpp` setzt `Diagram::margin` auf `0.0`. Damit beginnt der Rahmen bei `0/0`; XML-Feintuning am Rahmen kann nun ohne den geerbten 5-Einheiten-Komfortrand erfolgen.
- Verifikation in diesem Windows-Checkout: `git diff --check` sauber. Kein lokaler QET-Build ausgeführt, da hier kein eingerichtetes `build/baseline` mit Qt/CMake-Konfiguration vorhanden ist.

### 2026-09-28 - Rahmenkontrolle nach Nullrand
- Nach `Diagram::margin = 0.0` wurde die MAM-A3-Geometrie rechnerisch kontrolliert. Der Ursprung wandert von `5/5` nach `0/0`; Breite und Höhe des Rahmens selbst bleiben unverändert.
- Aus `examples/MAM_EPLAN_A3_Klemmenplan.qet` und `MAM_EPLAN_Klemmenplan_A3` ergibt sich aktuell: Rahmenursprung `0/0`, Breite `1511.5`, Gesamthöhe `1069.599998`, Kopfstreifen `23.4`, Zeichenfeldhöhe `960.199998`, Schriftfeldhöhe `86`.
- Codepfadprüfung: `BorderTitleBlock`, `CellRuler`, DXF/Export und Diagrammposition verwenden weiter `Diagram::margin`, laufen also konsistent auf den neuen Ursprung. Der einzige separate `5.0`-Wert im aktuellen Rahmenpfad ist der rechte Innenabstand der dynamischen Nachbarseitenzahl und kein äußerer Seitenrand.
- Windows-Verifikation nach Qt-/MSVC-/SDK-Einrichtung: Konfiguration und Build mit Qt `6.11.2 msvc2022_64` erfolgreich, erzeugte EXE `build/windows-msvc-app/qelectrotech.exe`, Versionsprobe `0.200.1-dev`. Qt-Laufzeit wurde mit `windeployqt` neben die EXE gelegt; zusätzlich wurde die zum lokalen SQLite-Link passende `sqlite3.dll` ergänzt.
- Frischer Windows-CLI-Export mit genau diesem Binary: `build/windows-msvc-app/qelectrotech.exe --export-png examples/MAM_EPLAN_A3_Klemmenplan.qet build/frame-check`; Ergebnis `build/frame-check/01_Durchlaufofen.png` (1513 x 1071 px). Sichtprüfung: obere linke Rahmenecke beginnt bei Bildursprung, Spaltenstreifen zeigt `0-9`, keine leere Kopfzelle vor `0`, Schriftfeld ist nicht abgeschnitten. Native GUI-Screenshot des Nutzers bestätigt ebenfalls, dass der Rahmen links/oben bündig bei `0` sitzt; die freie Fläche rechts/unten gehört zur Editor-Arbeitsfläche und ist kein Blattrand.
- In diesem Windows-Checkout fehlt weiterhin `Rahmenvorlage.png`; ein pixelweiser Vergleich gegen die Referenz bleibt daher offen. Für den aktuellen Eingriff ist der Nullrand aber build- und exportseitig bestätigt.

### 2026-09-28 - PDF-/Druckvorschau auf randlose MAM-Seite gestellt
- Nutzerprüfung bestätigte: Sprache ist deutsch, aber in der Druck-/PDF-Vorschau sitzt der Rahmen weiterhin mit großem Papierrand auf der Seite. Dieser Rand stammt nicht aus `Diagram::margin` oder `outerbordermargin`, sondern aus dem `QPrinter`-Seitenlayout und der Einpassung in die druckbare Fläche.
- Geändert: Für `QPrinter::PdfFormat` setzt `ProjectPrintWindow` jetzt A3 Landscape, `fullPage=true` und Seitenränder `0/0/0/0 mm`. Der Preview-Dialog zeigt die Vollseitenoption für PDF ebenfalls aktiv. Physische Drucker behalten ihre eigenen Drucker-/Hardware-Ränder.
- Verifikation Windows: Rebuild `build/windows-msvc-app --target qelectrotech` erfolgreich; `qelectrotech.exe --version` meldet `0.200.1-dev`; CLI-PNG-Export weiterhin erfolgreich nach `build/frame-check-pdfmargin/01_Durchlaufofen.png`; `git diff --check` sauber. Native GUI-PDF-Vorschau nach Neustart durch Nutzer erneut prüfen.

### 2026-09-28 - Strompfad-Kontext im Terminal/Potential-Report begonnen
- Punkt 1 des MAM-Fahrplans begonnen: Strompfade/Potentiale werden zuerst read-only sichtbar gemacht, ohne schon eine neue persistierte Strompfad-Ownership einzuführen.
- `--export-mam-terminal-potential` unterscheidet jetzt technische Rasterfelder (`from_grid`, `to_grid`, `grid_range`) von fachlichen Strompfadspalten (`from_path`, `to_path`, `path_range`).
- Die sichtbare Referenz folgt der WSCAD-/EPLAN-nahen Darstellung `/Seite.Spalte` in `from_reference`, `to_reference` und `reference_range`; die Leitung/Potentialbezeichnung bleibt separat in `wire_number`.
- Nutzerreferenzen konkretisieren die sichtbare Darstellung: am Pfeil steht gross der Potential-/Leitungsname wie `1L1`, `1N` oder `4L`, klein daneben die Gegenstelle als `Seite.Spalte`; Pfeile koennen seitlich oder nach unten zeigen, Spannungsangaben bleiben Zusatztext.
- Der zugehörige QtTest `tst_mam_terminal_potential_export` wurde um Header- und Plausibilitätsprüfungen für Raster, Pfadspalte und Referenzfelder erweitert.
- Verifikation Windows: App-Rebuild erfolgreich; direkter CLI-Export mit `tests/qttest/fixtures/workflow_exports_minimal.qet` erzeugt `build/mam-terminal-paths.csv` mit technischen Rastern wie `B0`/`C1`, fachlichen Pfaden wie `0`/`1` und Referenzen wie `/1.0`/`/1.1`; `qelectrotech.exe --version` meldet `0.200.1-dev`; `git diff --check` sauber. Eine separate Windows-Testkonfiguration mit `PACKAGE_TESTS=ON` scheiterte vor Testausführung an Catch2-FetchContent/Git-Erkennung in dieser Windows-CMake-Umgebung, nicht am geänderten Code.

### 2026-09-28 - Zweiblättriges Strompfad-Beispiel angelegt
- Neues Beispielprojekt `examples/MAM_Strompfade_2Seiten.qet` angelegt. Es basiert auf dem stabilen Workflow-Export-Fixture, enthält zwei Diagrammseiten `STROMPFADE SEITE 1` und `STROMPFADE SEITE 2` und unterscheidet die Leiterbereiche `W005-W011` sowie `W105-W111`.
- Seite 2 erhielt eigene Element-UUIDs und aktualisierte Leiterreferenzen, damit das Beispiel nicht nur eine naive Kopie mit identischen Betriebsmittelidentitäten ist.
- Verifikation Windows: `--export-mam-terminal-potential examples/MAM_Strompfade_2Seiten.qet build/mam-strompfade-2seiten.csv` exportiert 14 Leiter mit 0 Warnungen. Nach WSCAD-Korrektur zeigt der Export über beide Seiten `folio=1/2`, technische Raster wie `B0`/`C1`, fachliche Pfade wie `0`/`1` und Blatt-/Spaltenreferenzen wie `/1.0` und `/2.1`. Zusätzlich erzeugt `--export-png` zwei Seiten unter `build/mam-strompfade-2seiten-png/`.

### 2026-09-28 - Sichtbarer Strompfad-Fortfuehrungsprototyp
- Neue MAM-Pilot-Bibliothekselemente unter `elements/10_electric/10_allpole/100_folio_referencing/mam/` angelegt: horizontale Fortfuehrung rechts, horizontale Fortfuehrung links und Fortfuehrung nach unten.
- Die Elemente nutzen editierbare Zusatzfelder `potential`, `xref` und `voltage`, damit die sichtbare Darstellung aus Potentialname, Gegenstelle und optionaler Spannung aufgebaut werden kann, ohne das QET-Sonderfeld `label` zu missbrauchen.
- `examples/MAM_Strompfade_2Seiten.qet` enthaelt jetzt sichtbare Pfeil-Prototypen: Seite 1 zeigt `400VAC` mit `1L1` bis `1L3` nach rechts sowie `0VAC` mit `1N`/`2N` nach unten; Seite 2 zeigt passende eingehende Gegenstellen.
- Verifikation Windows: `--check-elements` fuer die drei neuen MAM-Elemente meldet 3 OK, 0 Warnungen, 0 Fehler. `--export-png` rendert zwei Seiten nach `build/mam-strompfade-visible-png/`; Crops `crop-page1-arrows.png` und `crop-page2-arrows.png` zeigen Pfeil, grossen Potentialnamen und kleinen Gegenstellenverweis. `--export-mam-terminal-potential` bleibt bei 14 Leitern und 0 Warnungen.
- Nutzerentscheidung: Strompfade sind fuer den ersten Stand i.O.; optischer Feinschliff erfolgt spaeter.
- Noch bewusst offen: automatische Platzierung, echte Verknuepfung zwischen Fortfuehrungspfeilen, Renummerierung nach Seiten-/Pfadaenderungen und Feintuning von Abstaenden/Typografie im finalen MAM-Rahmen.

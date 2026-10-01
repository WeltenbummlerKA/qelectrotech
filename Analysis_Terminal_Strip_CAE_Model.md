# Analysis: Norm- and CAE-Oriented Terminal Strip Formation

Date: 2026-09-30  
Status: binding analysis basis for the private MAM terminal-strip implementation

## Executive Summary

Professional CAE systems do not form terminal strips by concatenating a strip name and a terminal number into one visible text such as `-X5:11`.

The required model is separated:

- Terminal strip / device designation: `-X5`
- Terminal number / terminal designation: `11`
- Optional terminal connection point designations: e.g. `1`, `2`, `L`, `N`, `PE`, or article-/function-specific connection point names
- Optional physical position, level/deck, bridge, cable/core, counterpart, and article data

The strip designation identifies the common terminal-strip object. The terminal number identifies the terminal within that strip. In the schematic representation the common strip designation is normally shown once for the strip or terminal group, while the individual terminals display their own terminal numbers or connection point designations. Repeating `-X5` at every terminal is a presentation defect unless explicitly configured for a special report view.

For MAM, the current combined-label bridge (`-X5:1`, `-X5:2`, ...) is therefore only a temporary implementation aid. The target implementation must store and process strip BMK and terminal number separately.

## Source Basis And Boundaries

This report uses public information and observable CAE behavior as an architecture guide. It does not reproduce proprietary standard text or vendor-internal data models.

Public source basis:

- IEC 81346-1: reference designations and structuring principles for technical objects.
- IEC 61666: identification of terminals within systems.
- IEC 61082 series: preparation and presentation of electrotechnical documents.
- Public EPLAN documentation for terminal strips, terminal insertion, terminal management, terminal sorting, and terminal-strip properties.
- Public WSCAD and Zuken E3.series documentation on terminal browsers, multi-stage/multi-level terminals, terminal plans, and merging/grouping terminal objects.

## Normative Principles Relevant To Terminal Strips

### 1. Object Identification And Terminal Identification Are Different

IEC 81346 is about structuring systems and reference designations for objects. Public ISO/IEC descriptions state that a reference designation identifies an object so information about that object can be created, retrieved, and coordinated across documents.

IEC 61666 is specifically about identification of terminals within a system. Public IEC descriptions state that it establishes general principles for identifying terminals of objects within a system.

Consequence:

- `-X5` is the reference designation of the terminal strip object.
- `11` is not a second device BMK; it is the terminal designation within that strip.
- A terminal's visible identity in an electrotechnical document is a composition of object context and terminal designation, not necessarily a single stored string.

For MAM this means:

```text
TerminalStrip.reference_designation = -X5
Terminal.terminal_designation       = 11
Full address for reports/navigation = -X5:11
Schematic display                   = -X5 shown once, 11 shown at the terminal
```

### 2. Document Presentation Must Derive From Engineering Data

IEC 61082-oriented document preparation separates electrotechnical document views from the underlying engineering facts. A circuit diagram, connection document, terminal diagram, wiring list, and device list may present different subsets of the same objects.

Consequence:

- The schematic symbol does not own the whole terminal-strip truth.
- A terminal-strip plan must be generated from the same terminal-strip data as the schematic references.
- Hiding repeated `-X5` is not a manual text trick; it is a presentation rule for a grouped object.

### 3. The Terminal Strip Is A Structured Object, Not A Loose Symbol Row

A CAE terminal strip contains ordered terminals. A physical terminal can have one or more levels/decks and one or more connection points. Bridges, accessories, PE functions, cable cores, and counterparts attach to specific terminal positions, levels, or connection points.

Minimum structure:

```text
TerminalStrip (-X5)
  TerminalPosition 1
    TerminalNumber 1
    Real terminal / connection point(s)
  TerminalPosition 2
    TerminalNumber 2
    Real terminal / connection point(s)
  ...
```

For multi-level terminals:

```text
TerminalStrip (-X2)
  PhysicalTerminal 1
    Level L   -> designation 1L
    Level L0  -> designation 1L0
    Level PE  -> designation 1PE
```

The physical position and the electrical terminal/connection point designation must not be collapsed into one unstructured string.

## How Standard CAE Systems Form Terminal Strips

### EPLAN Pattern

Public EPLAN documentation describes terminal strips as terminal devices with a device tag. When a new terminal strip DT is specified, EPLAN can generate a new terminal strip. EPLAN's terminal management distinguishes terminal strip, terminal designation, device position, sort code, levels, and terminal-related reports.

Observed CAE pattern:

- Terminals with the same terminal strip device tag belong to the same terminal strip.
- The terminal designation identifies the terminal on that strip.
- Sorting/device position determines physical sequence.
- Multi-level terminals use level information and physical grouping, not just similar text labels.
- Terminal diagrams and line-up diagrams are generated from the managed terminal-strip object.

Implication for MAM:

```text
Common field:      terminal_strip_bmk = -X5
Per-terminal:      terminal_number = 1..n
Derived full key:  -X5:1, -X5:2, ...
Visible schematic: -X5 once, terminal numbers at the connection points
```

### WSCAD Pattern

Public WSCAD documentation for multi-stage terminals describes a terminal browser where terminal levels and terminal position numbers determine which connections belong together physically. Sorting can use alphabetic, coordinate, level, or mixed rules and affects terminal plans and cabinet placement.

Observed CAE pattern:

- Physical terminal identity is separate from the schematic symbol occurrence.
- Multi-stage/multi-level terminals are grouped by terminal position and level.
- Sorting is a managed terminal-strip operation, not an incidental drawing order.

Implication for MAM:

- A terminal needs a stable physical position inside the strip.
- A level/deck field is needed before multi-level terminals are credible.
- Display order must be data-driven and reusable by reports.

### Zuken E3.series Pattern

Public Zuken documentation describes terminal tables, terminal plans, and functions to merge terminals into multi-level terminals. Such operations preserve or reassign wiring relationships to the new terminal structure.

Observed CAE pattern:

- Individual terminal occurrences can be combined into a structured terminal object.
- Wiring, cable, shield, and counterpart relationships must survive grouping.
- The terminal plan is a generated view of the terminal data.

Implication for MAM:

- Grouping terminals into `-X5` must not destroy conductor/cable endpoint identity.
- The terminal-strip service must own assignment/grouping rules, not the visible overlay element.

## Required MAM Data Model

### Minimum Single-Level Model

```text
TerminalStrip
  uuid
  reference_designation = -X5
  installation/location/aspect fields, if present
  terminals ordered by physical_position

PhysicalTerminal
  uuid
  physical_position = 1..n
  article/manufacturer/type data
  levels / real terminals

RealTerminal
  uuid or stable identity
  terminal_number = 1..n
  connection_point_designation(s)
  function/type, e.g. generic, PE, fuse, disconnect
  placed schematic element UUID(s)
  connected conductor/cable/core/counterpart evidence
```

For the current simple case:

```text
Strip: -X5
PhysicalTerminal #1 -> terminal_number 1
PhysicalTerminal #2 -> terminal_number 2
...
PhysicalTerminal #11 -> terminal_number 11
```

### Required Separation Of Fields

Do not use one field for everything.

Bad temporary bridge:

```text
element.label = -X5:11
```

Target:

```text
element.strip_bmk       = -X5
element.terminal_number = 11
```

Optional derived values:

```text
full_terminal_key = -X5:11
display_group     = -X5
display_terminal  = 11
```

### Presentation Rule

The schematic renderer/report renderer decides what is shown:

```text
If several adjacent/related terminal symbols share strip_bmk = -X5:
  show "-X5" once for the group/strip
  show each terminal_number next to its terminal symbol
```

For a single isolated terminal, a configurable view may show either:

```text
-X5  o 11
```

or a fuller address:

```text
-X5:11
```

But the underlying data remains separated.

## Required Behavior When Adding An 11th Terminal

User action:

```text
Create a terminal.
Set BMK / terminal-strip designation = -X5.
Set terminal number = 11.
```

Expected CAE behavior:

1. The system recognizes that `-X5` already exists as a terminal strip.
2. The terminal is assigned to terminal strip `-X5`.
3. It becomes terminal position/number `11`, unless a different physical position/sort code is specified.
4. The schematic view extends the group to show terminal `11`.
5. The common `-X5` remains shown once for the strip/group.
6. The new terminal does not display another independent `-X5` label unless the user explicitly enables a verbose display mode.
7. Terminal-strip report/export includes the 11th terminal.
8. The terminal plan/line-up diagram updates from the same data.

Current QET/MAM behavior gap:

- `TerminalStripAssignmentService::assignSingleLevelByPrefix()` currently recognizes combined labels matching `<prefix>:<number>`.
- It does not yet read separate `strip_bmk` and `terminal_number` facts.
- It does not yet derive the visible terminal-strip row.
- It does not yet suppress repeated strip BMK in the schematic representation.

## Consequences For The Current QET/MAM Implementation

### What Must Be Treated As Temporary

The following are only implementation bridges:

- Combined label parsing: `-X5:1`
- A visual `x5_terminal_strip_row.elmt` overlay
- Hidden terminal carrier elements behind a manually drawn row
- Manual text placement to imitate a terminal-strip group

These may help with test data and export evidence, but they are not accepted CAE behavior.

### What Must Become First-Class

Required implementation concepts:

- Separate terminal-strip BMK field.
- Separate terminal number field.
- Terminal-strip grouping service using shared BMK.
- Stable physical order/sort code.
- Rendering rule that displays common BMK once.
- Terminal-strip report generated from the same grouped data.
- Later: levels/decks, bridges, PE/shield, cable/core, counterpart, article/accessory data.

### Suggested Incremental Implementation Slices

1. Add or standardize a source field for terminal-strip BMK on terminal elements.
2. Keep `terminal_number` as a separate field and stop deriving it from the suffix of `label`.
3. Update assignment logic to group free terminal elements by `terminal_strip_bmk`.
4. Preserve backward compatibility by optionally parsing legacy/temporary `-X5:11` only when separated fields are absent.
5. Add a focused test: ten terminals with `terminal_strip_bmk=-X5` and `terminal_number=1..10` produce one `TerminalStrip` named `-X5`.
6. Add test for adding terminal 11: same BMK and number 11 extends the same strip.
7. Add a schematic presentation rule: group label once, terminal numbers at each point.
8. Remove the visual overlay from accepted examples once native rendering exists.

## Acceptance Criteria

A MAM terminal-strip implementation is acceptable only when all of these hold:

- A terminal strip can be created from terminals carrying separate BMK and terminal number facts.
- `-X5` is stored as the terminal-strip/device designation, not only as a prefix inside a label string.
- Terminals `1..n` remain separate terminal numbers.
- Adding terminal `11` with BMK `-X5` assigns it to the existing strip.
- The schematic representation does not repeat `-X5` at every terminal.
- Reports and exports can still produce full keys such as `-X5:11`.
- The same data can drive a future terminal plan without reinterpreting drawing text.
- The old combined label path is documented as compatibility/transition, not as MAM target behavior.

## Public References

- ISO page for IEC 81346-1:2022, describing structuring principles and unambiguous reference designations: https://www.iso.org/standard/82229.html
- DIN overview of reference designation according to ISO/IEC 81346 series: https://www.din.de/de/referenzkennzeichnung-nach-der-normenreihe-iso-iec-81346-na-152-06-09-ga-695914
- IEC page for IEC 61666:2010, identification of terminals within a system: https://webstore.iec.ch/en/publication/5705
- IEC TC 3 document kinds page, describing IEC 61082 use for electrotechnical documents: https://tc3.iec.ch/tc-activity/current_document_kinds/
- EPLAN help, inserting terminals and terminal strip definitions: https://eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/terminalgui_h_klemmenzeichnen.htm
- EPLAN help, managing terminals: https://www.eplan.help/en-us/Infoportal/Content/Plattform/2.9/Content/htm/terminalgui_k_verwaltung.htm
- EPLAN API terminal strip properties, including full device tag and terminal-strip item count: https://eplan.help/en-us/Infoportal/Content/api/2026/Eplan.EplApi.DataModelu~Eplan.EplApi.DataModel.Properties%2BTerminalStrip.html
- EPLAN blog, generated terminal line-up diagrams and labelling information: https://www.eplan.com/us-en/blog/software/terminal-strips-easier-than-ever
- WSCAD multi-stage terminals in terminal browser: https://docu.wscad.com/HELP/English/WSCAD54/AT/11_Automatic_function/11-17Klemmenplan/Klemmen_Browser/Multi-stage_in_the_terminal_browser.htm
- Zuken E3.series terminal functions / terminal plan documentation: https://www.zuken.com/doc/e3/series/en/Content/reference/functions/Terminal%20Plan/terminals/terminal_functions.htm

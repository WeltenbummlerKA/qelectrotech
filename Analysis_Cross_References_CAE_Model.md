# Analysis: Querverweise im CAE-Modell

Date: 2026-10-01
Status: binding analysis basis for the private MAM cross-reference implementation

## Kurzfazit

Querverweise sind kein Textformat und kein reines Anzeigeproblem. In einem professionellen CAE-System sind Querverweise berechnete Beziehungen zwischen Fachobjekten und deren Darstellungen: Geraeteteile, Kontakte, Spulen, SPS-Kanaele, Klemmen, Potentialfortsetzungen, Kabel-/Aderziele, Funktionsgruppen, Seiten, Rasterzonen und Reports.

Fuer MAM bedeutet das:

- Ein Querverweis hat eine fachliche Beziehung als Quelle, nicht nur sichtbaren Text.
- Der sichtbare Querverweis ist eine Darstellung dieser Beziehung, z. B. `/2.5`, `3-B13`, `=A1+S1/4.2`, oder ein Kontaktspiegel.
- Unterschiedliche Querverweisarten brauchen getrennte Semantik: Geraet, Kontakt, Potential-/Signalfortsetzung, Paar-/Sternverweis, SPS-Uebersicht, Klemmen-/Anschlussplan, Kabel-/Aderziel, Report-/Listenverweis.
- Seiten-/Rasterposition, Pfadnummer, BMK, Potentialname, Signalname, Drahtnummer und Zielobjekt duerfen nicht in ein freies Textfeld verschmolzen werden.
- Jede manuelle sichtbare Kopie muss als `display copy` gelten und auf stale/conflict pruefbar sein.

## Source Basis And Boundaries

Diese Analyse nutzt oeffentliche Normbeschreibungen, oeffentliche CAE-Dokumentation und den aktuellen QET/MAM-Codebestand. Sie ersetzt keine lizenzierte Normpruefung.

Wichtige oeffentliche Quellen:

- IEC 61082-1: Regeln fuer Darstellung von Informationen in elektrotechnischen Dokumenten, inklusive Diagrammen, Zeichnungen und Tabellen.
- ISO/IEC 81346: Strukturierung und Referenzkennzeichnung technischer Objekte.
- IEC 61175: Signalbezeichnungen.
- EPLAN Electric P8: Cross-reference basics, device/contact/interruption-point/pair cross-references, plot-frame dependent display, interruption-point settings.
- Zuken E3.series: signal cross-references, sheet references, point-to-point, star, auto point-to-point, auto star, cross-references between views.
- AutoCAD Electrical: parent/child component cross-references, source/destination signal arrows, real-time/update commands.
- WSCAD: PLC cross-reference navigator and PLC Manager concepts.

Aktuelle QET/MAM-Codebasis:

- `CrossRefItem` rendert Master/Slave-, Kontakt- und PLC-bezogene Querverweisanzeigen.
- `XRefProperties` definiert Anzeigeformeln wie `%f-%l%c`, Position, Anzeigeart `cross` vs. `contacts`, Snap-Verhalten und Projekt-/Default-Einstellungen.
- `ContactCrossRefProjectionService` ist ein read-only Dienst fuer Master/Slave-/Kontaktzuordnung, `group_index`, Kapazitaet, Status und Warnungen.
- `PlcIoProjectionService` behandelt PLC-Master/Slave-Displaykopien bereits als read-only/stale-pruefbare Projektionen.
- `TerminalStripDrawer` zeigt Klemmenleisten-XRefs ueber `AssignVariables::genericXref()` an.
- Pilot-Strompfadpfeile besitzen derzeit Felder wie `potential`, `xref`, `voltage`; das ist nur Darstellung, kein fertiges Querverweis-Fachmodell.

## Normorientierte Grundprinzipien

### 1. Querverweise verweisen auf Objekte, nicht auf Texte

Ein Querverweis muss von einem identifizierbaren Quellobjekt zu einem identifizierbaren Zielobjekt bzw. zu einer Zielmenge fuehren. Sichtbarer Text wie `/4.7`, `3-B13` oder `(2-C5)` ist nur die formatierte Ausgabe.

Konsequenz:

```text
source_object_id
target_object_id / target_set_id
reference_kind
display_format
resolved_position
status
```

muessen getrennt bleiben.

### 2. Referenzkennzeichnung und Ortsreferenz sind getrennt

ISO/IEC-81346-nahe Objektkennzeichen beantworten: "Welches Objekt ist das?"

Seite, Raster, Spalte, Zeile, Strompfad oder Pfadnummer beantworten: "Wo finde ich die Darstellung?"

Beides darf kombiniert angezeigt werden, aber nicht kombiniert gespeichert werden.

### 3. Querverweise sind dynamisch

Wenn ein Ziel auf eine andere Seite verschoben wird, muss der Querverweis automatisch neu berechnet werden. Wenn Seiten umsortiert oder Raster/Plotrahmen geaendert werden, darf kein veralteter Text stehen bleiben.

Konsequenz:

- Querverweisziel = stabile Darstellung oder Fachobjekt-ID.
- Querverweistext = berechnetes Ergebnis aus Projektstruktur, Seite, Raster, Pfad und Formatregel.
- Manuelle Ueberschreibungen brauchen Warnstatus.

### 4. Verschiedene Querverweisarten haben verschiedene Kardinalitaeten

Nicht jeder Querverweis ist 1:1.

```text
device view             1:n  distributed functions of one device
coil/contact mirror     1:n  coil/master to contact slots/slaves
pair reference          1:1  source-target pair
star reference          1:n  source to many targets
signal continuation     1:1 / 1:n / chain, depending on project rule
PLC overview            1:n  module/channel overview to distributed I/O symbols
terminal strip          1:n  strip/terminal row to schematic terminals
cable/core target       1:n  connector/terminal to remote targets
report row              n:1  generated list row back to represented object
```

Ein generisches `xref_text`-Feld reicht deshalb nicht.

## Wie gaengige CAE-Systeme Querverweise behandeln

### EPLAN Pattern

EPLAN beschreibt Querverweise als automatisch eingefuegte Projektdaten waehrend der Schaltplanbearbeitung. Die Grundtypen umfassen:

- Geraetequerverweise zwischen verteilten Funktionen eines Geraets.
- Kontaktquerverweise im Kontaktspiegel.
- Unterbrechungsstellenquerverweise.
- Paarquerverweise.

EPLAN erkennt die Zugehoerigkeit verteilter Geraetefunktionen ueber gleiches Betriebsmittelkennzeichen und passende Funktionsdefinition. Die Anzeige haengt vom Plotrahmen, Projekt-/Geraeteeinstellungen und dem Anzeigeformat ab. Unterbrechungsstellen koennen nach Zeile oder Spalte, als Matrix, nur mit Seitennamen, mit voller Strukturkennung, mit Zielanzeige, Stern-/Paarlogik und Separatoren formatiert werden.

MAM-Konsequenz:

- `CrossReferenceKind` muss Geraet, Kontakt, Unterbrechungsstelle, Paar/Stern und PLC/Klemme unterscheiden.
- Gleiches BMK allein reicht nicht; die Funktionsdefinition bzw. fachliche Rolle gehoert zur Identitaet.
- Plotrahmen-/Rasterdaten sind Darstellungskontext, nicht Querverweisidentitaet.
- Kontaktspiegel ist eine strukturierte Darstellung von Kontaktgruppen, nicht eine Liste freier Zieltexte.

### Zuken E3.series Pattern

Zuken E3.series behandelt Signal Cross-References als Symbole, die eine Verbindung unterbrechen und auf derselben oder einer anderen Seite fortsetzen. Die Software unterscheidet Quelle/Ziel, Signal, laufende Nummer, Position, Ziel und Referenzstatus.

Wichtige Referenzarten:

- Point-to-point: eine Quelle zu einem Ziel.
- Star: Quelle zu mehreren Zielen bzw. mehrere Zielbeziehungen.
- Auto point-to-point: automatische Verknuepfung anhand identischer Signale und Reihenfolge/Signalfluss.
- Auto star: automatische Sternverweise anhand identischer Signale.
- Star unique: gezielt steuerbare Anzeige je verbundenem Referenzsymbol.

Zuken kennt ausserdem Querverweise zwischen verschiedenen Views, z. B. Device Views und Pin Views.

MAM-Konsequenz:

- Signal-/Potentialfortsetzungen brauchen eigene Objekte mit `source/destination`, `reference_type`, `signal_or_potential`, `sequence`, `target_position`.
- Automatische Verweise muessen bei Seitenumordnung und Verschieben neu aufloesen.
- Device-/Pin-/Klemmen-/Report-Views sind mehrere Darstellungen desselben Fachobjekts.

### AutoCAD Electrical Pattern

AutoCAD Electrical unterscheidet Komponentenquerverweise und Source/Destination-Signalpfeile. Parent/Child-Komponenten erhalten gegenseitige Referenzpositionen. Signalpfeile arbeiten mit einem Source/Destination-Konzept: Ein Wire Network erhaelt einen Source-Code, passende Destination-Pfeile verwenden denselben Code; beim Aktualisieren werden Zielreferenzen und Drahtnummern synchronisiert.

AutoCAD Electrical kann Cross-References in Echtzeit aktualisieren oder ueber Befehle wie `AEUPDATESIGREF` / Update Signal References nachziehen.

MAM-Konsequenz:

- Signalcode bzw. Fortsetzungs-ID muss von sichtbarem Referenztext getrennt sein.
- Ein Update-Lauf fuer Querverweise ist ein legitimes MAM-Konzept, auch wenn spaeter Online-Aktualisierung folgt.
- Wire number, signal code und Zielposition sind separate Fakten.

### WSCAD / PLC Manager Pattern

WSCAD fuehrt PLC-Querverweise ueber Manager-/Navigator-Sichten. SPS-Hauptmodule, Hilfselemente, Kanal-/Pin-/Adressdaten und Seitenbezug werden zusammen ausgewertet.

MAM-Konsequenz:

- PLC-Querverweise duerfen nicht aus sichtbaren PLC-Adressfeldern allein entstehen.
- PLC-Uebersicht und verteilte I/O-Symbole muessen ueber Kanal-/Anschlussidentitaet verbunden sein.

## Required MAM Data Model

### Fachobjekte

MAM sollte mindestens folgende Querverweisprojektion modellieren:

```text
CrossReference
  id
  kind                  # device, contact, continuation, pair, star, plc_io, terminal, cable_core, report
  source_object_id
  source_representation_id?
  target_object_id?
  target_set_id?
  relationship_id?      # contact assignment, channel binding, terminal membership, net continuation
  direction             # source, destination, bidirectional, overview, backreference
  cardinality           # 1:1, 1:n, n:1, chain, star
  signal_id?
  potential_id?
  net_id?
  reference_designation?
  display_format_id
  resolved_display_text
  resolved_target_position
  status                # ok, missing_target, ambiguous, stale, conflict, hidden, suppressed
  warnings[]
```

Darstellungen:

```text
ReferenceRepresentation
  id
  object_id
  representation_type   # schematic, overview, terminal_plan, report, panel, list
  folio
  page_identifier
  grid_row
  grid_column
  path_number?
  plot_frame_context?
  element_uuid?
  text_item_uuid?
```

Formatregeln:

```text
ReferenceDisplayFormat
  id
  kind
  formula               # e.g. page-path, page-grid, full structure + page + path
  include_structure
  include_target_label
  include_page
  include_grid
  include_path
  separator
  orientation
  placement_rule
```

### Was ausdruecklich getrennt bleiben muss

Nicht akzeptabel:

```text
xref = "=A1+S1/4.2 Motor_Run_FB"
```

Ziel:

```text
source.object_id           = uuid(coil -K1)
target.object_id           = uuid(NO contact -K1)
relationship.kind          = contact_assignment
target.refdes              = -K1
target.function_definition = NO auxiliary contact
target.folio               = 4
target.path                = 2
display.formula            = "%page.%path"
display.text               = "4.2"
```

Oder fuer Signalfortsetzung:

```text
continuation.signal_id     = Motor_Run_FB
continuation.reference_type = auto_point_to_point
source.arrow_id            = uuid(...)
destination.arrow_id       = uuid(...)
source.folio/path          = 1/7
destination.folio/path     = 3/2
display.text_at_source     = "/3.2"
display.text_at_target     = "/1.7"
```

## Querverweisarten fuer MAM

### 1. Geraetequerverweis

Verbindet mehrere Darstellungen/Funktionen desselben Betriebsmittels.

Beispiele:

- Spule und weitere Darstellungen desselben Schuetzelements.
- Schutzgeraet-Hauptteil und Hilfskontakte.
- Mehrteilige Geraetesymbole auf verschiedenen Seiten.

Identitaet:

```text
same reference designation + compatible function definition + shared device object id
```

### 2. Kontaktquerverweis / Kontaktspiegel

Verbindet Master/Spule mit Kontaktgruppen und verteilten Slave-Kontakten.

MAM-Regel:

- Kontaktspiegel zeigt deklarierte Kontaktgruppen und belegte/freie Plaetze.
- `group_index` ist aktueller QET-Uebergangsmechanismus, nicht endgueltige Fachidentitaet.
- Frueh-/Spaetkontakte, NO/NC/Wechsler, Anschlussnummern und Zielpositionen bleiben getrennte Fakten.

### 3. Potential-/Signalfortsetzung

Verbindet unterbrochene Netze/Potentiale/Signale ueber Seiten hinweg.

MAM-Regel:

- Pfeilsymbol ist Darstellung.
- Fortsetzungsbeziehung ist Fachobjekt.
- Projektregel entscheidet: point-to-point, chain, star, auto-by-signal, manual pair.

### 4. SPS-/PLC-Querverweis

Verbindet PLC-Uebersicht, Modul/Kanal, verteiltes I/O-Symbol, Adresse, symbolische Adresse und Feldgeraet.

MAM-Regel:

- Adresse ist nicht Primaerschluessel.
- Kanal-/Anschlussidentitaet verbindet die Darstellungen.
- Overview row und Stromlaufplansymbol sind Views desselben PLC-Fachobjekts.

### 5. Klemmen-/Anschlussquerverweis

Verbindet schematische Klemme, Klemmenleistenansicht, Anschlussliste, Zielgeraet, Bruecken und Potentiale.

MAM-Regel:

- Klemmenleisten-BMK und Klemmennummer bleiben separate Fakten.
- Querverweis auf Klemmenplan ist Darstellungskontext.
- Ziel-/Gegenstellenanzeige darf aus Verdrahtungsdaten berechnet werden, nicht manuell gepflegt.

### 6. Kabel-/Ader-/Zielquerverweis

Verbindet Draht/Ader/Leiter mit Quelle, Ziel, Kabel, Klemme, Stecker und Potential.

MAM-Regel:

- Wire number, cable core, potential, source target und schematic reference bleiben getrennt.
- Zielreferenz kann im Plan, Klemmenplan, Kabelplan oder Report erscheinen.

### 7. Report-/Listenquerverweis

Verbindet generierte Listenzeilen mit den dargestellten Fachobjekten.

MAM-Regel:

- Reportzeile ist nicht das Objekt.
- Klickbare Navigation aus Liste/CSV/UI zur Planstelle braucht stabile `object_id` + `representation_id`.

## QET/MAM Ist-Zustand

Vorhanden:

- `XRefProperties` kann Anzeigeformat, Position, Display-Art und Projektdefault speichern.
- `CrossRefItem` beobachtet Projekt-/Seiten-/Linkaenderungen und rendert Master/Slave-/Kontakt-/PLC-nahe Anzeigen.
- `ContactCrossRefProjectionService` bietet read-only Fakten und Warnungen fuer Kontaktzuordnung.
- `PlcIoProjectionService` bietet read-only Fakten und Warnungen fuer PLC-Master/Slave-Projektionen.
- `TerminalStripDrawer` zeichnet Klemmenleisten-XRefs anhand der platzierten Terminalelemente.
- `AssignVariables::genericXref()` und Formeln wie `%f-%l%c` liefern bestehende Positions-/Foliodarstellung.

Grenzen:

- Es gibt kein einheitliches `CrossReference`-Fachmodell ueber alle Domains.
- Kontakt-, PLC-, Terminal-, Report- und Strompfad-Querverweise sind getrennte Inseln.
- `group_index` ist fuer Kontakte/PLC eine Uebergangsmechanik, keine allgemeine Querverweisidentitaet.
- Sichtbare XRef-Formeln sind Anzeige, aber derzeit oft naeher an der Implementierung als am Fachmodell.
- Stale/conflict-Diagnostik existiert punktuell, aber nicht als generischer Querverweisstatus.
- Fortsetzungs-/Unterbrechungsstellen fuer Potentiale/Signale sind noch nicht als native Fachobjekte modelliert.

## Required Behavior When Editing

### Fall 1: Zielobjekt wird auf eine andere Seite verschoben

Erwartet:

- Querverweis bleibt an der fachlichen Beziehung.
- Sichtbarer Text wird aus neuer Seite/Raster/Pfad neu berechnet.
- Keine manuelle Textkopie bleibt still veraltet.

### Fall 2: Betriebsmittel-BMK wird geaendert

Erwartet:

- Alle Darstellungen desselben Fachobjekts aktualisieren ihre Anzeige.
- Querverweise bleiben ueber Objekt-ID/Beziehung erhalten.
- Wenn mehrere Objekte nun kollidieren, entsteht eine Diagnose, keine stille Neuverknuepfung.

### Fall 3: Kontakt wird einem anderen Kontaktplatz zugeordnet

Erwartet:

- Kontaktspiegel aktualisiert belegten/freien Platz.
- Zielposition und Anschlussnummern bleiben getrennt.
- Doppelte Belegung desselben Kontaktplatzes wird gewarnt.

### Fall 4: Potentialfortsetzung wird umsortiert

Erwartet:

- Projektregel entscheidet Point-to-point, Chain oder Star.
- Quelle/Ziel-Pfeile behalten Fachidentitaet.
- Seiten- und Pfadaenderungen berechnen die sichtbaren Referenzen neu.

### Fall 5: PLC-Uebersicht und verteiltes Symbol widersprechen sich

Erwartet:

- Kanal-/Anschluss-ID entscheidet die Beziehung.
- Adresse/Funktionstext/symbolische Adresse werden als Anzeige-/Datenkopien geprueft.
- Widerspruch erzeugt `stale` oder `conflict`, nicht zwei Kanaele.

## Acceptance Criteria

Eine MAM-Querverweisumsetzung ist erst akzeptabel, wenn:

- Querverweise als Beziehungen zwischen Fachobjekten/Darstellungen modelliert werden.
- Geraete-, Kontakt-, Potential-/Signal-, PLC-, Klemmen-, Kabel-/Ader- und Reportquerverweise unterscheidbar sind.
- Sichtbarer Querverweistext aus Formatregeln und Positionen berechnet wird.
- Seite, Raster, Strompfad/Pfadnummer, Strukturkennung, Ziel-BMK, Potential, Signal und Drahtnummer separat auswertbar bleiben.
- 1:1-, 1:n-, n:1-, Stern- und Kettenbeziehungen modellierbar sind.
- Stale, missing target, ambiguous target, duplicate assignment und conflicting display copy diagnostiziert werden koennen.
- Seitenumordnung, Elementverschiebung, BMK-Aenderung und Plotrahmen-/Rasteraenderung Querverweise aktualisieren oder warnen.
- Kontaktspiegel und PLC-Uebersicht nicht als Textlisten, sondern als Views auf Fachbeziehungen funktionieren.
- Bestehende QET-Dienste `ContactCrossRefProjectionService` und `PlcIoProjectionService` als read-only Bruecken erhalten bleiben, bis ein allgemeines CrossReference-Modell ausreichend belegt ist.

## Suggested Implementation Slices

1. Analysebericht als Leitplanke versionieren.
2. Bestehende `Decision_Contact_CrossRef_Ownership.md` und `Spec_Contact_CrossRef_ReadOnly_Service.md` gegen diese breitere Querverweis-Taxonomie abgleichen.
3. Read-only `CrossReferenceProjectionService` entwerfen, der vorhandene Inseln nur sammelt: contact assignments, PLC projections, terminal-strip xrefs, pilot continuation arrows.
4. Erste Ausgabe als `--export-mam-cross-reference` CSV: kind, source, target, folio/path/grid, display text, status, warnings, source service.
5. Diagnosen zuerst: missing target, ambiguous target, stale text, duplicate assignment, unresolved continuation, out-of-frame/invalid grid.
6. Danach einzelne Domain vertiefen: Kontaktspiegel oder Potentialfortsetzung, nicht alles gleichzeitig.
7. Erst nach stabiler Projektion UI-Update/Write-back: automatische XRef-Textaktualisierung, klickbare Navigation, Format-Editor, Reparaturbefehle.

## Public References

- IEC 61082-1:2014 preparation of documents used in electrotechnology: https://webstore.iec.ch/en/publication/4469
- ISO/IEC 81346-1:2022 overview: https://www.iso.org/standard/82229.html
- IEC 61175-1:2015 signal designations: https://webstore.iec.ch/en/publication/22490
- EPLAN cross-references basics: https://eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/xessettingsgui_k_grundlagen.htm
- EPLAN interruption point settings: https://eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/xessettingsgui_d_einstellungenprojektabbruchstellen.htm
- Zuken E3.series sheet references / signal cross-references: https://www.zuken.com/doc/e3/series/en/Content/reference/dialogs/project_mode/reference_properties/IDD_DIALOG_SHEET_REFERENCE.htm
- Zuken E3.series cross-references between views: https://www.zuken.com/doc/e3/series/en/Content/reference/functions/References/cross_references_between_views.htm
- Autodesk AutoCAD Electrical cross-referencing: https://help.autodesk.com/cloudhelp/2022/ENU/AutoCAD-Electrical/files/GUID-1C1CDDA5-BD10-43AD-9AD8-2287834B059D.htm
- Autodesk AutoCAD Electrical signal arrows: https://help.autodesk.com/cloudhelp/2024/ENU/AutoCAD-Electrical/files/GUID-BD454148-4281-4D87-B228-9C92C4823657.htm
- Autodesk AutoCAD Electrical update signal references: https://help.autodesk.com/cloudhelp/2025/ENU/AutoCAD-Electrical/files/GUID-3C4CE083-4B7D-45B7-B2D5-94D1EB19BAD4.htm
- WSCAD PLC cross reference navigator: https://docu.wscad.com/HELP/English/WSCAD54/AT/21_PLC_Manager/PLC_cross_reference_navigator_.htm

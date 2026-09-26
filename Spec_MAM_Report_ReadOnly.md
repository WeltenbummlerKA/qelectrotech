# Spec - MAM Read-only Report Layer

Date: 2026-09-26
Status: short field/audience decision for the private MAM fork

## Zielgruppe und Nutzung

Der kombinierte MAM-Report ist zuerst fuer interne MAM-CAE-Pruefung gedacht:

- Projekt-/CAE-Verantwortliche sehen schnell, welche PLC-, Kontakt-/CrossRef- und spaeter Terminal-/Potential-Fakten aus dem geladenen QET-Projekt ableitbar sind.
- Arbeitsvorbereitung und Schaltschrank-/Inbetriebnahme-nahe Pruefung bekommen eine einfache CSV-Grundlage fuer Vergleich, Plausibilitaet und Rueckfragen.
- Entwickler nutzen denselben Report als deterministische Regression ueber vorhandene Projektzustandsdaten.

Der Report ist kein Editor, kein Reparaturwerkzeug und keine neue Fach-Datenbank.

## Grenze

Die MAM-Report-Schicht bleibt read-only:

- zuerst CLI/CSV, mit Semikolon als Trenner wie `--export-mam-plc-io`;
- keine UI-Aenderung;
- keine Persistenz-, XML-Schema-, Device/Core- oder Migrationsaenderung;
- keine automatische Korrektur von Links, `group_index`, PLC-Kopien, Terminaldaten oder Potentialen;
- keine Packaging-/Runtime-Annahme.

Die Report-Schicht darf nur vorhandene Projektdaten und bereits getestete Projection/Validation Services lesen.

## CSV-Familien

Die kombinierte Report-Familie bleibt fachlich getrennt und kann spaeter durch einen Sammelaufruf gebuendelt werden:

- `mam-plc-io`: bestehender PLC-IO-Kanalreport aus `PlcIoProjectionService`.
- `mam-contact-crossref`: naechster kleiner Report ueber `ContactCrossRefProjectionService`, mit Master-/Slave-Zuordnung, Gruppenindex, Gruppenmetadaten und Warnungen.
- `mam-terminal-potential`: naechster Report fuer Terminal-, Leiter- und Potential-Kontext, aus bestehenden Live-Graph- und Export-/Datenbank-Fakten, ohne Terminal-Strip-Ownership zu behaupten.
- `mam-summary`: spaeterer Uebersichtsreport mit Zaehlern und Warnungsgruppen, nicht als Ersatz fuer die Detail-CSVs.

Ein zukuenftiger kombinierter Export darf mehrere CSV-Dateien schreiben oder eine klar benannte Summary ergaenzen. Er soll die Detailtabellen nicht zu einer breiten Misch-Tabelle verschmelzen.

## Gemeinsame IDs und Felder

Gemeinsame Feldkonventionen:

- UUID-Felder ohne geschweifte Klammern, analog `--export-mam-plc-io`.
- `folio` als QET-Folioindex, leer oder `-1` nur wenn kein Folio aufloesbar ist.
- Labels als beobachtete Elementlabels aus dem geladenen Projektzustand.
- Indizes wie `io_index` und `group_index` bleiben nullbasiert, weil sie direkt auf bestehende QET-/Projection-Strukturen zeigen.
- Mehrwertige Felder wie Terminal-Labels oder Warning-Listen werden innerhalb eines CSV-Feldes mit stabilen Trennern zusammengefasst.

Empfohlene gemeinsame Spaltenfamilien:

- Objektidentitaet: `*_label`, `*_uuid`, `*_folio`.
- Fachreferenz: `io_index`, `group_index`, `type`, `subtype`, `terminal_count`, `terminal_labels`.
- Status: `status`, `warnings`.

## Warning- und Status-Konventionen

Status bleibt einfach und nicht-blockierend:

- `OK`: keine Warnung in der Zeile.
- `WARNING`: mindestens eine read-only Projection-/Validation-Warnung.

Warnungen muessen deterministisch formuliert sein und duerfen keine Reparatur behaupten. Vorhandene Service-Warnungen werden nicht still uebersetzt oder zusammengefuehrt, solange dadurch Fachdetails verloren gingen.

## Bewusst getrennt

Getrennt bleiben:

- PLC IO und Contact/CrossRef, obwohl beide `group_index` nutzen.
- Terminal/Potential/Terminal-Strip-Kontext, bis dessen eigene Reportfelder spezifiziert sind.
- Detail-CSV und Summary.
- Report-/Export-Schicht und QET-Core/Device-/XML-/UI-Ebene.
- Regressionsevidence und echte Runtime-/Packaging-/GUI-QA.

## Terminal/Potential-CSV-Entscheidung

Ziel und Nutzung:

- `mam-terminal-potential` dient der internen MAM-CAE-Pruefung, ob Leiter, Endpunkte und elektrische Potentialgruppen aus dem geladenen Projekt konsistent auswertbar sind.
- Die CSV soll einen stabilen, menschenlesbaren Vergleich zwischen Potential-/Netzsicht und vorhandener Verdrahtungsliste ermoeglichen.
- Der erste Slice bleibt eine Detailtabelle pro Leiter/Conductor, nicht Terminal-Strip-, Kabel- oder Summary-Report.

Source-of-truth-Grenzen:

- Elektrische Potentiale kommen aus dem bestehenden Live-Graph (`Conductor::relatedPotentialConductors()`), wie bei `--export-nets`.
- Leiter-/Endpunktidentitaet kommt aus den bestehenden `Conductor`/`Terminal`/`Element`-Objekten und darf mit Datenbank-/Wiring-Export-Fakten verglichen werden.
- `--export-wiring`, `--export-cables`, `--export-wires` und `--export-nets` bleiben bestehende Referenzflaechen; der MAM-Report darf diese Fakten zusammenfuehren, aber keine Reparatur oder neue Autoritaet erzeugen.
- Terminal strips bleiben bewusst getrennt: `QETProject`/`TerminalStrip`-Mitgliedschaft ist dokumentierte read-only Evidenz, aber nicht automatisch elektrische Potential-Wahrheit.

CLI-/Dateivorschlag:

- Neuer CLI-Flag: `--export-mam-terminal-potential <project.qet> <output.csv>`.
- Format: Semikolon-CSV wie die bestehenden MAM-Reports.
- Familie: eigenstaendige Detaildatei `mam-terminal-potential`, spaeter optional durch einen Sammelaufruf neben PLC und Contact/CrossRef exportierbar.

Pflichtfelder und IDs fuer den ersten Slice:

- `wire_number`: sichtbarer Leitertext bzw. Potential-Leiternummer, leer erlaubt.
- `conductor_uuid`: bestehende Leiter-UUID ohne geschweifte Klammern.
- `folio`: QET-Folioindex des Leiters.
- `from_element_label`, `from_element_uuid`, `from_terminal`: erster Leiterendpunkt.
- `to_element_label`, `to_element_uuid`, `to_terminal`: zweiter Leiterendpunkt.
- `potential_wire_number`: kleinster/nutzbarer nichtleerer Leitertext der Potentialgruppe, analog Netzexport.
- `potential_conductor_count`: Anzahl Leiter in der Potentialgruppe.
- `potential_terminal_count`: Anzahl beobachteter Terminals in der Potentialgruppe.
- `status`, `warnings`.

Warning-/Status-Konventionen:

- `OK`: Zeile ist auswertbar und hat keine beobachtete Inkonsistenz.
- `WARNING`: mindestens eine read-only Auffaelligkeit.
- Erste deterministische Warnungen: leere `conductor_uuid`, einseitig/fehlender Endpunkt, leere Endpunktlabels, und leerer `wire_number` wenn die Potentialgruppe keine nutzbare Leiternummer hat.
- Warnungen bleiben Zeilentext, mit ` | ` getrennt, ohne Reparaturbehauptung.

Bewusst nicht enthalten:

- Terminal-Strip-Mitgliedschaft, Bruecken und Ebenen.
- Kabel-/Adermodell, Busmodell, Artikel-/Klemmenleisten-Fertigungsausgabe.
- Potential-Isolator-Editorlogik, UI, Rendering, Klickflaechen.
- XML-Schema-, Device-/Core-, Persistenz-, Migrations- oder Autorepair-Entscheidung.

## Implementierter Terminal/Potential-Slice

Der erste kleine Slice ist `mam-terminal-potential` als CLI/CSV-Export ueber die bestehende Live-Graph-Potentiallogik und Leiterendpunktdaten.

Minimalfelder:

- `wire_number`, `conductor_uuid`, `folio`
- `from_element_label`, `from_element_uuid`, `from_terminal`
- `to_element_label`, `to_element_uuid`, `to_terminal`
- `potential_wire_number`, `potential_conductor_count`, `potential_terminal_count`
- `status`, `warnings`

Nicht in diesem Slice:

- Terminal-strip facts, bridge semantics, cable/core ownership, UI, persistence, XML schema, runtime/package work.

## Naechster implementierbarer Slice

Ein naechster kleiner Slice kann fokussierte Warning-Coverage fuer `mam-terminal-potential` ergaenzen, etwa eine temporaere Fixture-Variante mit leerer Leiternummer oder fehlendem Endpunkt, ohne die Reportfelder zu erweitern.

## Vorheriger implementierter Slice

Der vorherige kleine Slice war `mam-contact-crossref` als CLI/CSV-Export ueber den vorhandenen `ContactCrossRefProjectionService`.

Minimalfelder:

- `master_label`, `master_uuid`, `master_folio`
- `slave_label`, `slave_uuid`, `slave_folio`
- `group_index`, `group_index_resolves`
- `group_type`, `group_subtype`, `group_contact_count`, `group_terminal_count`, `group_terminal_labels`
- `slave_contact_type`, `slave_contact_subtype`, `slave_contact_count`
- `duplicate_group_assignment`
- `status`, `warnings`

Nicht in diesem Slice:

- CrossRef-Rendering, Geometrie, Klickflaechen oder UI.
- Persistenz-/XML-/Schema-Aenderung.
- Master-Summary-Tabelle.
- Terminal-/Potentialkontext.

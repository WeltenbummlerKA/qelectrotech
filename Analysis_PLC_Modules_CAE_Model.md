# Analysis: SPS-/PLC-Module im CAE-Modell

Date: 2026-10-01  
Status: binding analysis basis for the private MAM PLC module implementation

## Kurzfazit

SPS-/PLC-Module duerfen im MAM-Fork nicht als reine Tabellenzeichnung mit Adress-Texten verstanden werden. Ein PLC-Modul ist ein technisches Geraet bzw. eine Baugruppe mit CPU/Rack/Slot/Modul-/Kanalstruktur, Anschluss-/Klemmenpunkten, I/O-Kanaelen, Adressen, symbolischen Adressen, Funktionstexten, Signalen, Potential-/Versorgungsanschluessen und Querverweisen zu verteilten Schaltplansymbolen.

Die sichtbare SPS-Uebersicht ist nur eine Darstellung. Die verteilten SPS-Ein-/Ausgangssymbole im Stromlaufplan sind weitere Darstellungen derselben PLC-Kanaele bzw. Anschlussfunktionen. Adresse, Symbolname, Funktionstext, Anschlussbezeichnung, Kanalbezeichnung, Signaltyp und Verdrahtung duerfen nicht in ein unstrukturiertes Freitextfeld gepresst werden.

Fuer MAM bedeutet das:

- Ein PLC-Modul hat eine eigene Identitaet, getrennt von seinen Kanaelen.
- Ein PLC-Kanal ist nicht automatisch identisch mit genau einem Anschluss; ein Kanal kann mehrere Anschluss-/Versorgungspunkte haben.
- Die PLC-Adresse ist wichtig, aber nicht die einzige Identitaet eines PLC-Anschlusses.
- Uebersichts- und Stromlaufplansymbole muessen dasselbe PLC-Fachobjekt referenzieren.
- Slave-/verteilte PLC-Texte wie `plc_address` sind Anzeige-/Projektionskopien, nicht die fachliche Wahrheit.

## Source Basis And Boundaries

Diese Analyse nutzt oeffentliche Normbeschreibungen und oeffentliche CAE-Dokumentation als Architekturleitplanke. Sie ersetzt keine Normlizenz und kopiert keine proprietaeren Normtexte.

Relevante oeffentliche Quellen:

- IEC 61131 series: programmable controllers; IEC 61131-2 beschreibt Anforderungen an PLC/PAC-Geraete und Peripherie, IEC 61131-3 ist die verbreitete Programmiersprachenbasis.
- ISO/IEC 81346: Strukturierung und Referenzkennzeichnung technischer Objekte.
- IEC 61082: Darstellung elektrotechnischer Dokumente.
- IEC 61175: Signalbezeichnungen.
- EPLAN: PLC connection points, PLC channels, symbolic addresses, PLC structure, addressing and PLC data exchange.
- WSCAD: PLC Manager, Modul-/Adress-/Funktionstextverwaltung.
- Zuken E3.series: PLC I/O address generation and PLCBridge data exchange.
- SEE Electrical / AutoCAD Electrical als Vergleich fuer Rack-/I/O-Tabellen, Adressierung und Uebersichts-/Schaltplanbezug.

Parallel dazu wurden zwei getrennte Recherchelinien ausgewertet:

- Norm-/Strukturrecherche: IEC 61131, IEC/ISO 81346, IEC 61082, IEC 61175, IEC 60617 und QElectroTech-Element-/Terminalmodell.
- CAE-Systemvergleich: EPLAN Electric P8, WSCAD PLC Manager, Zuken E3.series/PLCBridge und SEE Electrical/SEE Electrical Expert.

Beide Linien kommen zum gleichen Architekturpunkt: Das PLC-Fachmodell muss stabiler sein als jede einzelne sichtbare Symbolinstanz.

## Normorientierte Grundprinzipien

### 1. PLC-Hardware, I/O-Kanal und Signal sind verschiedene Ebenen

IEC 61131 betrachtet programmierbare Steuerungen als industrielle Steuergeraete mit Ein-/Ausgaengen und Peripherie. IEC 61175 trennt Signalbezeichnungen vom physischen Traeger. ISO/IEC 81346 trennt Referenzkennzeichnung technischer Objekte.

Konsequenz fuer MAM:

```text
PLC device / station      = z. B. -A1 oder =CTRL-A1
Rack / station / bus node = physische oder logische Einbaustruktur
Module / card             = z. B. DI16, DO16, AI4
Channel                   = logischer I/O-Kanal
Connection point          = Klemme/Pin am Modul
PLC address               = z. B. %I0.0, I4.3, Q2.0
Symbolic address          = z. B. Motor_Run_FB
Signal / potential        = elektrische/logische Verbindung
Wire/cable/core           = physische Verdrahtung
```

Diese Fakten duerfen nicht durch ein einzelnes `label` oder eine einzelne sichtbare Tabellenzeile ersetzt werden.

### 2. Dokumentdarstellung muss aus dem Fachmodell ableitbar sein

IEC-61082-orientierte Dokumente koennen Schaltplan, SPS-Uebersicht, Anschlussplan, Klemmenplan, I/O-Liste und Verdrahtungsliste getrennt darstellen. Alle muessen dieselben PLC-Daten referenzieren.

Konsequenz:

- Die PLC-Uebersicht ist keine zweite Kopie der PLC-Wahrheit.
- Ein verteilter PLC-Anschluss im Schaltplan ist keine neue PLC-Adresse.
- Reports muessen aus den PLC-Kanaelen und Anschlussbeziehungen entstehen, nicht aus abgetipptem Tabellen-Text.

### 3. Adresse ist wichtig, aber nicht alleinige Identitaet

Oeffentliche EPLAN-Dokumentation beschreibt, dass ein PLC-I/O-Anschluss zu einer PLC-Karte oder einem Kanal gehoert und ueber PLC-Box-DT, Steckerbezeichnung und Anschlussbezeichnung eindeutig identifiziert wird; die Adresse ist nicht zwingend identifizierend und nicht zwingend bei der Planung erforderlich. Spaeter koennen Adressen automatisch vergeben oder aus Zuordnungslisten gelesen werden.

Konsequenz:

- `address = %I0.0` darf nicht das einzige Primaerschluesselfeld sein.
- Ein Kanal braucht eine stabile Identitaet unabhaengig von spaeterer Adressaenderung.
- Automatische Adressierung und Import aus SPS-Zuordnungslisten muessen moeglich bleiben.

## Wie gängige CAE-Systeme SPS-Module behandeln

### EPLAN Pattern

EPLAN unterscheidet PLC-Struktur, PLC-Boxen/Karten, PLC-Anschlusspunkte, Kanaele, Adressen, symbolische Adressen, Funktionstexte und Darstellungsarten. Eine Karte kann in verteilten Funktionen dargestellt werden; eine zusaetzliche Uebersichtsdarstellung ist moeglich. PLC-I/O-Anschlusspunkte gehoeren zu einer PLC-Karte oder einem Kanal. Ein Kanal kann mehrere zugehoerige Anschlusspunkte besitzen, z. B. einen aktiven Signalanschluss plus Sensorversorgung.

Wichtige oeffentliche EPLAN-Aussagen:

- Ein PLC-I/O-Anschluss gehoert zu Karte oder Kanal.
- Ein Anschluss hat immer eine Anschlussbezeichnung und oft eine Anschlussbeschreibung.
- Eindeutigkeit kommt aus PLC-Box-DT, Steckerbezeichnung und Anschlussbezeichnung.
- Adresse ist nicht identifizierend und nicht zwingend.
- Fuer PLC-Querverweise zaehlen fachliche Identitaet der PLC-Karte, Funktionsdefinition, Steckerbezeichnung und Anschlussbezeichnung; die sichtbare PLC-Adresse darf nicht die Querverweis-Identitaet sein.
- Kanaele haben wichtige Daten fuer Zuordnungslisten, z. B. symbolische Adresse und Funktionstext.
- Bei kombinierten Kanalanschlusspunkten hilft die Kanalbezeichnung zur Zuordnung.

MAM-Konsequenz:

```text
PLCModule.reference_designation
PLCConnectionPoint.plug_designation
PLCConnectionPoint.connection_point_designation
PLCChannel.channel_designation
PLCChannel.address
PLCChannel.symbolic_address
PLCChannel.function_text
```

muessen getrennt auswertbar bleiben.

### WSCAD Pattern

WSCAD fuehrt PLC-Informationen im PLC Manager. Der Manager zeigt und bearbeitet u. a. Symbolname, Funktionstext, Adresse und Vorschau der Planstelle. Startadressen und Adressgruppen eines Moduls koennen geaendert werden; bei Gruppen koennen Einzeladressen innerhalb des Moduls angepasst werden.

Der PLC Manager arbeitet dabei sichtbar mit einer PLC-Projektdatenbank: Hauptmodule beschreiben die Baugruppe, Hilfselemente bzw. Schaltplansymbole stellen einzelne oder gruppierte I/Os dar, und Detailansichten verbinden Kanal, Pin, Adresse, Funktionstext, Baugruppe, Slot und Seite.

MAM-Konsequenz:

- PLC-Module brauchen eine Manager-/Navigator-Sicht, nicht nur Symbole.
- Startadresse, Einzeladresse, Funktionstext und Planstelle sind verschiedene Felder.
- Adressaenderungen muessen auf Kanal-/Gruppenebene nachvollziehbar sein.
- Hauptmodul/Uebersicht und verteiltes I/O-Symbol duerfen keine getrennten Geraete erzeugen.

### Zuken E3.series Pattern

Zuken E3.series kann PLC-I/O-Adressen automatisch generieren. Dafuer wird eine PLC-Startadresse an den Pin des ersten I/O-Elements vergeben; bei unterschiedlichen I/O-Typen wie Input/Output werden getrennte Startadressen fuer die ersten I/O-Elemente verwendet. In Project mode werden daraus physische PLC-Adressen fuer I/O-Pins erzeugt. Zuken PLCBridge dient dem bidirektionalen Austausch von PLC-Daten mit SPS-Programmiersoftware und umfasst u. a. Hardware module type, item designation, physical addresses, symbolic addresses, I/O comments, start address, pins and configuration, and module layout.

MAM-Konsequenz:

- Startadresse und physische Kanaladresse sind getrennte Fakten.
- Input-/Output-Bereiche koennen verschiedene Adresszaehler haben.
- PLC-Daten muessen import-/exportfaehig zur Programmiersoftware sein.
- Modul-Layout und Pin-Konfiguration gehoeren zur PLC-Fachstruktur.

### SEE Electrical / AutoCAD Electrical Pattern

SEE Electrical und AutoCAD Electrical zeigen das gleiche Grundmuster in einfacherer Form: PLC-Racks/Module, I/O-Adressen, Funktionstexte, Zuordnungslisten und Darstellungen im Schaltplan bzw. auf Uebersichtsseiten sind miteinander gekoppelt. Adressen koennen automatisch nummeriert und in Reports ausgewertet werden.

Auch die dort beworbenen Echtzeitlisten fuer PLC-I/Os, Komponenten, Klemmen, Kabel/Adern, Verbindungen und Dokumente setzen voraus, dass Reports aus referenzierten Fachobjekten entstehen und nicht aus voneinander unabhaengigen Textfeldern.

MAM-Konsequenz:

- Auch ein einfaches MAM-Modell muss Adressierung, I/O-Liste und Schaltplandarstellung synchron halten.
- UI/Report darf nicht aus voneinander unabhaengigen Textkopien bestehen.

## Required Conceptual Separation

| Ebene | Bedeutung | Typische Attribute | Nicht gleichsetzen mit |
|---|---|---|---|
| PLC station/controller | CPU oder logische Steuerungsstation | BMK/Referenzkennzeichen, Hersteller, Bus/System | I/O-Modul, Symbolinstanz |
| Rack/bus node/slot | Einbau- oder Busstruktur | Rack-ID, Slot, Einbauort | Adresse, freier Text |
| Module/card | konkrete Baugruppe | Artikel, DI/DO/AI/AO/CPU, Startadresse, Layout | Kanal, Adresse |
| Channel/I/O point | fachlicher I/O-Kanal | Kanalnummer, Richtung, Signalart, Status | Pin, Draht, Potential |
| Address | SPS-Adresse | `%I0.0`, `%Q2.1`, Herstellerformat | Primaerschluessel, Signalname |
| Symbolic address | SPS-/Programmiername | `Motor_Run_FB` | Potential, physischer Draht |
| Connection point/pin | elektrischer Anschluss | Stecker, Anschlussbezeichnung, Klemmenlabel | Kanaladresse |
| Signal | Informationsinhalt | Funktionssignal nach IEC-61175-Logik | Potential, Klemme |
| Potential/net | elektrischer Zusammenhang | `24VDC`, `0V`, `PE`, geschaltetes Potential | Signalname |
| Representation | Plan-/Reportdarstellung | Seite, Symbol, Tabelle, Koordinate | Fachobjekt selbst |

Dieser Trenner ist die direkte Entsprechung zu den Klemmen- und Strompfadentscheidungen: Das sichtbare Zeichen ist eine Projektion, die CAE-Wahrheit liegt im Fachobjekt und seinen Beziehungen.

## Required MAM Data Model

### Fachobjekte

MAM sollte mindestens diese Objekte bzw. Projektionen fuehren:

```text
PLCStation / Controller
  id
  reference_designation
  manufacturer
  type_designation
  bus_system?
  cpu?
  comment

PLCRack / BusNode
  id
  station_id
  rack_designation
  location
  slot_count?

PLCModule / Card
  id
  station_id
  rack_id?
  slot
  reference_designation
  part_number
  module_type        # DI, DO, AI, AO, mixed, communication, power, CPU
  channel_count
  start_addresses
  layout / macro reference

PLCChannel
  id
  module_id
  channel_index
  channel_designation
  direction          # input, output, bidirectional, power, communication
  signal_kind        # digital, analog, universal, bus, diagnostic
  address
  symbolic_address
  function_text
  comment
  datatype
  status

PLCConnectionPoint
  id
  channel_id?
  module_id
  plug_designation
  connection_point_designation
  connection_point_description
  terminal_label
  signal_type
  active_io_point
  power_supply_role?

PLCRepresentation
  id
  represented_object_id
  representation_type  # schematic/distributed, overview, single-line, report
  element_uuid
  folio
  position

PLCSignalBinding
  channel_id
  signal_id?
  potential_id?
  field_device_ref?
  terminal_strip_ref?
  conductor_evidence?
```

### Was ausdruecklich getrennt bleiben muss

Nicht akzeptabel:

```text
label = "%I0.0 Motor running DI1"
```

Ziel:

```text
module.refdes              = -A1
module.slot                = 3
channel.channel_index      = 0
channel.address            = %I0.0
channel.symbolic_address   = Motor_Run_FB
channel.function_text      = Motor running feedback
connection_point.designation = 1
connection_point.plug        = X1
signal.name               = Motor_Run_FB
representation.type       = overview / schematic
```

Abgeleitet:

```text
visible overview row
visible PLC cross-reference
PLC I/O list row
TIA/E3/WSCAD exchange row
```

## Automatische Bildung von SPS-Uebersichten und Querverweisen

### Uebersicht vs. verteilte Darstellung

Ein PLC-Kanal kann gleichzeitig erscheinen:

- in der PLC-Uebersicht als Tabellen-/Kartenzeile;
- im Stromlaufplan als verteilter PLC-Anschluss;
- in einer I/O-Liste;
- in einer Verdrahtungs-/Klemmenliste.

Alle Darstellungen muessen dasselbe `PLCChannel`-/`PLCConnectionPoint`-Fachobjekt referenzieren.

Minimalregel:

```text
Wenn ein verteiltes SPS-Symbol dieselbe Modul-ID + Anschlussbezeichnung bzw.
Kanal-ID wie ein Uebersichtseintrag hat:
  zeige in beiden Darstellungen Querverweise
  synchronisiere Anzeige von Adresse, symbolischer Adresse und Funktionstext
  markiere abweichende Kopien als stale
```

### Adressierung

Adressierung darf automatisch oder manuell sein:

```text
module.start_address[input]  = %I0.0
module.start_address[output] = %Q0.0
channel[0].address           = %I0.0
channel[1].address           = %I0.1
...
```

Regeln:

- Adresse kann spaeter geaendert werden, ohne die Kanalidentitaet zu verlieren.
- Doppelte Adressen muessen warnen, ausser Projektregel erlaubt Mehrfachdarstellung desselben Kanals.
- Leere Adresse kann in frueher Planung erlaubt sein, muss aber in Reports diagnostizierbar bleiben.
- Unterschiedliche Hersteller-/SPS-Adressformate duerfen nicht ohne deklarierte Regel hart validiert werden.

### Kanal mit mehreren Anschlusspunkten

Beispiel 3-Leiter-Sensor:

```text
Channel DI0
  signal input point
  +24V sensor supply point
  0V sensor supply point
```

Regeln:

- Nur ein Anschluss ist der aktive I/O-Signalpunkt.
- Versorgungspunkte gehoeren fachlich zum Kanal, sind aber keine separaten I/O-Adressen.
- Potential-/Klemmen-/Leiterdaten muessen pro Anschluss erhalten bleiben.

## QET/MAM Ist-Zustand

Vorhandene MAM-Basis:

- `ElementData::PlcMasterData::ios` enthaelt PLC-Master-I/O-Zeilen.
- `ElementData::PlcIO` enthaelt derzeit u. a. Typ, Adresse, Funktionstext, Kommentar, CrossRef, TerminalCount und Terminal-Labels.
- `group_index` verbindet verteilte Slave-Symbole mit einer Master-I/O-Zeile.
- `PlcIoProjectionService` erzeugt read-only Projektionen und Warnungen.
- `--export-mam-plc-io` exportiert eine erste MAM-PLC-IO-CSV.
- Slave-Felder `plc_type`, `plc_address`, `plc_function`, `plc_comment`, `plc_tc`, `plc_t1`..`plc_t4` werden bereits als Kopien/Stale-Evidence behandelt.

Bekannte Grenzen:

- Es gibt noch kein eigenstaendiges PLC-Modul-/Rack-/Slot-/Kanal-Fachmodell.
- `group_index` ist aktuell Zuordnungsmechanik, nicht endgueltige PLC-Kanalidentitaet.
- Adresse und Kanal werden noch zu stark aus Master-Tabellenzeilen gelesen.
- Hersteller-/Adressformat-/TIA-/GSD-/EDS-Logik ist bewusst nicht implementiert.
- PLC-Uebersicht und verteilte Darstellung sind noch keine voll synchronisierten CAE-Ansichten.
- Terminal-/Potentialbezug ist Kontext, nicht PLC-owned truth.

## Required Behavior When Editing A PLC Module

### Fall 1: Neuer DI-Kanal wird eingefuegt

Benutzer fuegt an Modul `-A1`, Slot 3, Kanal 11 einen digitalen Eingang hinzu.

Erwartetes CAE-Verhalten:

1. Das Modul bleibt `-A1` Slot 3.
2. Ein neuer `PLCChannel` mit Index/Kanalbezeichnung 11 entsteht.
3. Adresse wird nach Projektregel vergeben oder bleibt bewusst leer.
4. Symbolische Adresse und Funktionstext sind getrennte Felder.
5. Uebersicht und verteilte Schaltplandetails zeigen denselben Kanal.
6. Report/Export enthaelt genau einen Kanal, nicht zwei Textkopien.

### Fall 2: Adresse wird geaendert

Benutzer aendert `%I0.7` auf `%I1.0`.

Erwartetes CAE-Verhalten:

- Kanalidentitaet bleibt stabil.
- Sichtbare Kopien in Uebersicht und verteilter Darstellung aktualisieren sich.
- PLC-Export nutzt die neue Adresse.
- Querverweise und Verdrahtung bleiben am Kanal, nicht an der alten Adresse.

### Fall 3: Uebersicht und verteiltes Symbol widersprechen sich

Uebersicht sagt `%I0.3`, verteiltes Symbol zeigt `%I0.4`.

Erwartetes CAE-Verhalten:

- Das ist ein `stale display copy` oder `conflicting representation`.
- Das System darf nicht stillschweigend zwei Kanaele erzeugen.
- Die authoritative Quelle muss eindeutig sein oder der Nutzer muss eine Synchronisation bestaetigen.

### Fall 4: Mehrere Anschlusspunkte pro Kanal

Ein analoger Eingang oder 3-Leiter-Sensor besitzt Signal+, Signal-, Versorgung und Bezugspotential.

Erwartetes CAE-Verhalten:

- Ein Kanal kann mehrere `PLCConnectionPoint`-Objekte besitzen.
- Nur der aktive I/O-Anschluss traegt die Kanaladresse.
- Versorgungspunkte bleiben verdrahtbar und potentialrelevant.
- Reports koennen Kanal- und Anschlussdaten getrennt ausgeben.

## Acceptance Criteria

Eine MAM-SPS-Modulumsetzung ist erst akzeptabel, wenn:

- PLC-Geraet/Station, Rack/Slot, Modul, Kanal, Anschluss, Adresse, symbolische Adresse, Funktionstext, Signal und Darstellung getrennt auswertbar sind.
- Eine PLC-Adresse nicht die einzige Identitaet eines Kanals ist.
- Uebersichts- und Schaltplansymbole dasselbe PLC-Fachobjekt referenzieren.
- Slave-/verteilte `plc_*`-Felder als Anzeige-/Projektionskopien behandelt und auf Stale-Zustand pruefbar sind.
- Ein Kanal mehrere Anschluss-/Versorgungspunkte haben kann.
- Leere, doppelte, out-of-range oder formatwidrige Adressen diagnostiziert werden koennen, ohne voreilig herstellerspezifisch zu blockieren.
- Aenderungen an Adresse/Funktionstext/symbolischer Adresse alle sichtbaren Darstellungen und Exporte konsistent aktualisieren koennen.
- PLC-IO-Reports, PLC-Uebersicht und Verdrahtungs-/Potentialkontext getrennte Sichten derselben Daten bleiben.
- Das bestehende `PlcIoProjectionService`-Modell als read-only Uebergang dokumentiert bleibt und nicht als endgueltiges PLC-Device-Core-Modell gilt.

## Suggested Implementation Slices

1. Analysebericht als Leitplanke versionieren.
2. Bestehende `Spec_PLC_IO_Semantics_ReadOnly.md` gegen diese Analyse abgleichen.
3. `PlcIoProjectionService` weiter read-only halten, aber Felder konzeptionell auf `PLCModule`, `PLCChannel`, `PLCConnectionPoint`, `PLCRepresentation` mappen.
4. Einen Testfall fuer getrennte authoritative Masterdaten und stale slave display copies beibehalten/erweitern.
5. Naechster Read-only Slice: PLC-Uebersicht vs. verteilte Darstellung als zwei Repräsentationen desselben Kanals pruefen.
6. Warnung fuer doppelte Adresse innerhalb gleicher PLC-Adressdomäne vorbereiten, ohne herstellerspezifische Syntax zu erzwingen.
7. Danach erst Schreiblogik: Synchronisieren von verteilten `plc_*`-Anzeige-/Formelfeldern aus Masterdaten.
8. Spaeter: natives PLC-Modul-/Rack-/Slot-/Kanal-Fachmodell mit Import/Export-Schnittstelle zu SPS-Programmiersystemen.

## Public References

- IEC 61131-2:2017 programmable controllers equipment requirements and tests: https://webstore.iec.ch/en/publication/31007
- ISO/IEC 81346-1:2022 overview: https://www.iso.org/standard/82229.html
- IEC TC 3 document kinds and IEC 61082 overview: https://tc3.iec.ch/tc-activity/current_document_kinds/
- IEC 61175-1:2015 signal designations: https://webstore.iec.ch/en/publication/22490
- EPLAN PLC connection points: https://eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/plcgui_k_spsanschluesse.htm
- EPLAN PLC channels: https://eplan.help/en-us/Infoportal/Content/Plattform/2022/Content/htm/plcgui_k_kanaele.htm
- EPLAN symbolic addresses: https://eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/plcgui_k_symbolischeadressen.htm
- EPLAN addressing PLC connection points: https://eplan.help/en-us/infoportal/content/plattform/2022/content/htm/plcgui_k_adressierung.htm
- EPLAN structure of PLCs: https://www.eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/plcgui_k_prinzip.htm
- EPLAN PLC master data preparation for PLC data exchange: https://www.eplan.help/techtipps/en-US/SPS/TechTip-Preparation-of-master-data-for-PLC-data-exchange.pdf
- WSCAD PLC Manager functions: https://docu.wscad.com/HELP/English/WSCAD54/AT/21_PLC_Manager/PLC_Manager_functions.htm
- WSCAD PLC Manager interface: https://docu.wscad.com/HELP/English/WSCAD54/AT/21_PLC_Manager/PLC_Manager_interface_.htm
- Zuken E3.series automatic addresses for PLC components: https://www.zuken.com/doc/e3/series/en/Content/reference/functions/PLC/Addressing_PLC_components.htm
- Zuken E3.PLCBridge PLC data exchange: https://www.zuken.com/us/product/e3series/electrical-schematic-design/plc-information-exchange/

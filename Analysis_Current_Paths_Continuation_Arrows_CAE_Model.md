# Analysis: Strompfade, Potentialfortsetzungen und Strompfadpfeile im CAE-Modell

Date: 2026-10-01  
Status: binding analysis basis for the private MAM current-path and continuation-reference implementation

## Kurzfazit

Strompfade, Potentialfortsetzungen und Strompfadpfeile duerfen im MAM-Fork nicht als freie Pfeilgrafik mit Text verstanden werden. Die Zeichnung ist nur die sichtbare Projektion eines Fachmodells aus Anschlusspunkten, Verbindungen, Netzen, Potentialen, Signalen, Leiterabschnitten, Fortsetzungsobjekten und berechneten Querverweisen.

Der Pfeil ist nicht die elektrische Verbindung. Der Pfeil zeigt eine Fortsetzung bzw. Unterbrechungsstelle einer Verbindung, eines Netzes, eines Signals oder eines Potentials. Seine sichtbare Beschriftung wird aus stabilen Daten berechnet: Potential-/Signalname, Zielseite, Strompfad/Koordinate, Richtung, Referenzmodus und optionaler Darstellungsvorgabe.

Fuer MAM bedeutet das:

- Potentialname, Signalname, Draht-/Leiternummer und sichtbarer Pfeiltext sind getrennte Fakten.
- Folio-/Strompfad-/Rasteradresse ist ein Querverweisziel, nicht die Identitaet des Potentials.
- Querverweise muessen berechnet werden und bei Seiten-, Raster-, Namens- oder Richtungswechseln aktualisierbar sein.
- Ein Pfeilsymbol ohne eigenes Fachobjekt ist nur ein grafischer Platzhalter und nicht CAE-standardgerecht.

## Source Basis And Boundaries

Diese Analyse nutzt oeffentliche Normbeschreibungen und oeffentliche CAE-Dokumentation als Architekturleitplanke. Sie ersetzt keine Normlizenz und kopiert keine proprietaeren Normtexte.

Relevante oeffentliche Quellen:

- IEC 61082: Regeln fuer die Erstellung elektrotechnischer Dokumente und Dokumentarten.
- ISO/IEC 81346: Strukturierungsprinzipien und Referenzkennzeichnung technischer Objekte.
- IEC 61175-1: Signalbezeichnungen, einschliesslich Stromversorgungssignalen; klare Trennung zwischen Informationsaspekt eines Signals und physischer Realisierung.
- EPLAN: Potentiale/Signale, Unterbrechungspunkte, Potentialdefinitionspunkte und automatische Querverweise.
- WSCAD: leitungsbezogene Querverweise, Referenzalgorithmus ueber Seite/Koordinate/Richtung offener Leitungsenden.
- Zuken E3.series: Signal Cross-References mit Point-to-Point, Star, Auto Point-to-Point, Auto Star und Star Unique.
- SEE Electrical und AutoCAD Electrical als Vergleich: Potential-/Signalpfeile bzw. Source-/Destination-Arrows mit gekoppeltem Referenzcode.

## Normorientierte Grundprinzipien

### 1. Dokumentdarstellung ist nicht Fachidentitaet

IEC 61082-orientierte Dokumentation legt fest, wie Informationen in elektrotechnischen Dokumenten verstaendlich dargestellt werden. Daraus folgt fuer MAM: Ein Stromlaufplan, eine Leiterliste, ein Anschlussplan und ein Querverweis sind verschiedene Darstellungen derselben technischen Fakten.

Konsequenz:

- Die Grafiklinie ist nicht allein die elektrische Wahrheit.
- Die Pfeilbeschriftung ist nicht allein die Potentialidentitaet.
- Eine spaetere Umnummerierung von Seiten oder Strompfaden darf die elektrische Identitaet nicht veraendern.

### 2. Referenzkennzeichen, Signalbezeichnung und Potential sind verschiedene Ebenen

ISO/IEC 81346 adressiert Objektstruktur und Referenzkennzeichen. IEC 61175 adressiert Signalbezeichnungen und trennt den Informationsaspekt eines Signals von seiner physischen Realisierung.

Konsequenz:

- Ein Betriebsmittel-BMK, z. B. `-K1`, ist nicht dasselbe wie ein Signalname.
- Ein Potentialname, z. B. `24V`, `0V`, `L1`, `PE`, ist nicht dasselbe wie eine Drahtnummer.
- Ein Strompfadverweis, z. B. `/2.4`, ist nicht dasselbe wie ein Potential.
- Ein Pfeil mit Text ist eine Darstellung eines Fortsetzungsobjekts, nicht das Fachobjekt selbst.

### 3. Signal/Potential, Leiter und Fortsetzung muessen getrennt modelliert werden

Minimal notwendige Trennung:

```text
Potential / Signal
  fachlicher Name und elektrische/logische Kontinuitaet

Net / Connection
  topologische Verbindung zwischen Anschlusspunkten

ConductorSegment / WireSegment
  physischer oder zeichnerischer Leiterabschnitt mit Geometrie und Drahtdaten

Continuation / Interruption Point
  Fortsetzungsobjekt an einem offenen Leitungsende oder Potentialpunkt

CrossReference
  berechnete Beziehung zwischen Fortsetzungsobjekten

Display
  Pfeilform, Textformat, Position, Farbe, Layer, Sichtbarkeit
```

## Wie gängige CAE-Systeme damit umgehen

### EPLAN Pattern

EPLAN unterscheidet Potentiale, Signale, Verbindungen und Unterbrechungspunkte. Unterbrechungspunkte werden genutzt, um eine Verbindung, ein Netz oder ein physisches/logisches Potential an anderer Stelle fortzusetzen. Querverweise zwischen passenden Unterbrechungspunkten werden automatisch gebildet. EPLAN unterscheidet u. a. Stern- und Kettenquerverweise; die Darstellung haengt von Projekteinstellungen, Plotrahmen, Struktur und Koordinaten ab.

Wichtige Konsequenzen:

- Der Name eines Unterbrechungspunktes kann dem logischen Potentialnamen entsprechen, ist aber nicht bloss freier Pfeiltext.
- Quelle/Ziel werden nicht dauerhaft als reine Symbolklassen gespeichert; das System kann die Beziehung automatisch bestimmen.
- Fehlt ein Gegenstueck, ist das ein diagnostizierbarer Fehler.
- Querverweistext ist Anzeige, nicht Primaeridentitaet.

### WSCAD Pattern

WSCAD bildet leitungsbezogene Querverweise an offenen Leitungsenden. Der Referenzalgorithmus verwendet Seite, Koordinaten und Richtung offener Leitungsenden; bei Zielverdrahtung kommen Leitungsname und Anschluss-/Verbindungsbezeichnung hinzu. WSCAD unterscheidet elektrische Verbindungslinien von freier Grafik.

Wichtige Konsequenzen:

- Offene Leitungsenden muessen geometrisch sauber sein.
- Richtung und Blattreihenfolge beeinflussen, welches Gegenstueck gefunden wird.
- Mehrdeutige gleichnamige Enden benoetigen Regeln oder manuelle Paarung.
- Grafische Linien ohne elektrische Semantik duerfen nicht in Querverweise einbezogen werden.

### Zuken E3.series Pattern

Zuken E3.series nutzt Signal Cross-References als eigene Symbole fuer unterbrochene und an anderer Stelle fortgesetzte Verbindungen. Es gibt Referenzmodi wie Point-to-Point, Star, Auto Point-to-Point, Auto Star und Star Unique. Automatische Referenzen werden ueber gleiche Signale, Referenztyp, Blattreihenfolge und Signalfluss gebildet.

Wichtige Konsequenzen:

- Source/Destination ist Teil eines Fortsetzungsmodells, nicht nur Pfeilform.
- Point-to-Point ist 1:1; Star erlaubt 1:n bzw. n:1.
- Automatische Modi benoetigen klare Regeln, wann ein Gegenstueck gefunden wird.
- Manuelle Steuerung bleibt notwendig, sobald mehrere gleichnamige Fortsetzungen existieren.

### SEE Electrical / AutoCAD Electrical Pattern

SEE Electrical und AutoCAD Electrical zeigen dasselbe Grundprinzip in einfacherer Form: Potential-/Signalpfeile oder Source-/Destination-Arrows werden ueber Namen, Produktindex, Signalcode oder Referenzcode gekoppelt; Querverweistext wird automatisch synchronisiert.

Wichtige Konsequenzen:

- Ein sichtbarer Pfeilcode dient als Kopplungsmerkmal, aber das System fuehrt daraus eine Querverweisbeziehung.
- Bei mehr als zwei gleichen Namen braucht es Paarindex, Produktindex, Gruppen-ID oder explizite Zielauswahl.
- Praefixe wie `to` / `from` oder Anzeigeformate sind Darstellungsattribute.

## Required MAM Data Model

### Fachobjekte

MAM sollte mindestens diese Objekte bzw. Projektionen fuehren:

```text
Potential
  id
  name                 # z. B. 24V, 0V, L1, PE
  potential_type       # L, N, PE/PEN, +, M, -, SH, generic
  voltage              # optional, z. B. 24VDC, 230VAC
  default_wire_props   # Farbe, Querschnitt, Layer/Stil optional

Signal
  id
  name                 # z. B. START_ENABLE, MOTOR_RUN
  potential_id?        # optional; nicht jedes Signal ist ein Versorgungspotential

Net
  id
  potential_id?
  signal_id?
  pin_ids
  wire_segment_ids

WireSegment
  id
  page_id
  geometry
  net_id?
  wire_number?
  cable_core_id?
  display_style

Pin / ConnectionPoint
  id
  element_uuid
  terminal_name
  terminal_number
  position

Continuation
  id
  page_id
  position
  direction            # left/right/up/down/in/out
  name                 # sichtbarer Fortsetzungsname, oft Potential/Signal
  potential_id?
  signal_id?
  mode                 # auto-point-to-point, manual-pair, auto-star, manual-star
  pair_group_id?
  sort_code?

CrossReference
  derived_from_continuation_ids
  target_page
  target_path_or_grid
  target_coordinate
  display_text
  status               # ok, unresolved, ambiguous, stale

DisplayOverride
  continuation_id
  symbol_variant
  text_format
  prefix
  visibility
```

### Was ausdrücklich nicht vermischt werden darf

Nicht in ein einziges Textfeld pressen:

```text
Potentialname + Drahtnummer + Zielseite + Strompfad + Pfeilrichtung
```

Stattdessen:

```text
potential.name       = 24V
wire.number          = W105
continuation.name    = 24V
crossref.target      = /2.4
display.text         = 24V /2.4
```

## Automatische Bildung von Strompfadpfeilen und Querverweisen

### Minimalregel fuer Point-to-Point

```text
Wenn genau zwei Continuation-Objekte denselben Signal-/Potentialbezug und denselben Modus auto-point-to-point haben:
  bilde einen 1:1-Querverweis
  berechne Zielseite und Zielpfad aus Dokumentstruktur und Position
  zeige an beiden Seiten den Zielverweis
```

### Minimalregel fuer mehr als zwei gleichnamige Fortsetzungen

```text
Wenn mehr als zwei Continuation-Objekte denselben Namen haben:
  wenn Modus auto-star:
    ein Source/Star-Source verweist auf alle Ziele
    jedes Ziel verweist auf die Source
  sonst wenn pair_group_id gesetzt:
    bilde Paare/Gruppen anhand dieser ID
  sonst:
    Status = ambiguous
    keine stillschweigend falsche Paarung erzeugen
```

### Richtung und Dokumentreihenfolge

Die Pfeilrichtung ist eine Leserichtung bzw. Fortsetzungsrichtung, nicht zwingend physikalische Stromrichtung. Trotzdem ist sie fuer automatische Paarung hilfreich:

- rechts ausgehender Pfeil sucht bevorzugt spaetere Positionen/Seiten;
- links eingehender Pfeil sucht bevorzugt fruehere Positionen/Seiten;
- nach unten kann den naechsten Strompfad oder eine definierte vertikale Fortsetzung bedeuten;
- manuelle Paarung uebersteuert Heuristik.

### Anzeigeformat

Das Anzeigeformat muss konfigurierbar sein:

```text
/2.4
2.4
=Funktion+Ort/2.4
nach /2.4
von /1.7
```

Das Format ist Darstellung. Die gespeicherte Beziehung bleibt die Continuation-/CrossReference-Beziehung.

## QET/MAM Ist-Zustand

QET hat bereits nutzbare Primitiven:

- `Conductor` als Verbindung zwischen Terminals.
- `relatedPotentialConductors()` fuer Potential-/Netzverfolgung.
- Leiter-/Conductor-Eigenschaften und sichtbare Leitertexte.
- Folio-/Report-Elemente und Potentialauswahl-Dialoge.
- MAM-Report `--export-mam-terminal-potential` mit `wire_number`, Endpoint-Fakten, `from_grid`, `to_grid`, `from_path`, `to_path`, `from_reference`, `to_reference`.
- Erste sichtbare Pilot-Elemente fuer Strompfadpfeile mit Feldern `potential`, `xref`, `voltage`.

Die Grenzen sind ebenso klar:

- Es gibt noch kein eigenstaendiges `Continuation`-Fachobjekt.
- Die sichtbaren Pfeil-Prototypen sind Symbol-/Textobjekte, keine authoritative Fortsetzungsbeziehungen.
- `from_reference`/`to_reference` im Report sind read-only Kontext, keine automatisch synchronisierte Pfeillogik.
- Folio-/Rasterkoordinaten duerfen nicht als Potentialidentitaet missverstanden werden.
- Bei mehr als zwei gleichnamigen Fortsetzungen fehlt aktuell eine belastbare Paar-/Star-Logik.

## Required Behavior When Editing A Plan

### Fall 1: gleicher Potentialname auf zwei Seiten

Benutzer setzt auf Seite 1 einen ausgehenden Fortsetzungspfeil `24V` und auf Seite 2 einen eingehenden Fortsetzungspfeil `24V`.

Erwartetes CAE-Verhalten:

1. Beide Pfeile werden als Continuation-Objekte erfasst.
2. Beide referenzieren dasselbe Potential oder Signal `24V`.
3. Das System bildet einen 1:1-Querverweis.
4. Seite 1 zeigt z. B. `24V` und klein `/2.4`.
5. Seite 2 zeigt z. B. `24V` und klein `/1.7`.
6. Wird Seite 2 verschoben, wird nur der sichtbare Querverweis aktualisiert; die Potentialidentitaet bleibt `24V`.

### Fall 2: drittes gleichnamiges Potential

Benutzer setzt eine dritte Fortsetzung `24V`.

Erwartetes CAE-Verhalten:

- Entweder Projektregel `auto-star`: alle Ziele werden an einer Source gesammelt.
- Oder Projektregel `chain`: Vor-/Nachfolge wird ueber Seiten-/Positionsreihenfolge gebildet.
- Oder manuelle Paarung ist erforderlich.
- Ohne eindeutige Regel muss ein Warnstatus `ambiguous continuation` entstehen.

### Fall 3: Pfeiltext wird manuell geaendert

Benutzer aendert nur sichtbaren Text am Pfeil.

Erwartetes CAE-Verhalten:

- Wenn der Text ein abgeleiteter Wert ist, darf er nicht die Potentialidentitaet aendern.
- Wenn der Benutzer den fachlichen Namen aendert, muss die Continuation-Beziehung neu berechnet werden.
- Anzeige-Override und Fachwert muessen getrennt bleiben.

## Acceptance Criteria

Eine MAM-Strompfad-/Potentialfortsetzungsumsetzung ist erst akzeptabel, wenn:

- Potential/Signal, Leiterabschnitt, grafische Linie, Fortsetzungsobjekt und Querverweis getrennt auswertbar sind.
- Ein Pfeil nicht nur freie Grafik mit Text ist, sondern ein Fortsetzungsobjekt mit stabiler ID.
- Gleiche Potential-/Signalfortsetzungen automatisch Querverweise bilden, sofern die Regel eindeutig ist.
- Mehrdeutige gleichnamige Fortsetzungen nicht stillschweigend falsch gepaart werden.
- Die sichtbare Zieladresse bei Seiten-/Strompfadverschiebung neu berechnet werden kann.
- Pfeilrichtung und Dokumentreihenfolge als Paarungshinweise verwendet werden koennen, aber nicht die elektrische Identitaet ersetzen.
- Querverweistext aus Zielseite/Zielpfad/Zielkoordinate berechnet wird.
- Potentialname und Signalname getrennt von Drahtnummer, Kabelader, BMK und Seitenkoordinate bleiben.
- Reports getrennt erzeugbar sind: Potentialliste, Leiterliste, Continuation-/Querverweisliste.
- Der alte Pilotansatz mit Feldern `potential`, `xref`, `voltage` als Darstellungsprototyp dokumentiert bleibt und nicht als akzeptiertes Fachmodell gilt.

## Problem Analysis Of The Failed Strompfad Attempts

Die bisherigen Strompfadversuche sind nicht am sichtbaren Pfeilsymbol selbst gescheitert, sondern an der falschen Schichtung der Fachlogik.

Der wesentliche Fehler war, dass sichtbare Pfeile und sichtbare Texte zu frueh als Loesung behandelt wurden. Ein Pfeil mit den Feldern `potential`, `xref` und `voltage` kann zwar eine brauchbare Planansicht erzeugen, ist aber noch kein CAE-gerechtes Strompfad-/Potentialfortsetzungsmodell. In einem CAE-System ist der Pfeil nur die Darstellung eines Fortsetzungsobjekts. Die gespeicherte Wahrheit muss getrennt beantworten:

- welches Potential oder Signal fortgesetzt wird;
- an welchem Anschluss-, Leiter- oder Fortsetzungspunkt die Unterbrechung liegt;
- welche Fortsetzungspunkte fachlich zusammengehoeren;
- welche Reihenfolge oder Kette zwischen mehreren gleichnamigen Fortsetzungen gilt;
- welche Zieladresse daraus berechnet wird;
- welcher sichtbare Text daraus angezeigt wird.

Die fehlgeschlagenen Ansaetze haben diese Ebenen zu stark vermischt:

- `xref` wurde als naheliegende Kopplungsinformation betrachtet, obwohl es nur sichtbare Darstellung ist.
- Potentialname, Drahtnummer, Seiten-/Rasteradresse, Strompfadnummer und Zielverweis waren nicht streng genug getrennt.
- Es gab keine stabile Ketteninformation wie Ketten-ID, Reihenfolge, Pair-/Group-ID oder explizite Source-/Destination-Fachbeziehung.
- Bei genau zwei gleichen Potentialpfeilen ist eine sichere 1:1-Projektion moeglich; bei drei oder mehr gleichnamigen Fortsetzungen waere eine Kette aus Seiten-/Positionsreihenfolge nur eine Heuristik, solange keine Kettenregel oder Kettenidentitaet gespeichert ist.
- Build- und Qt-Laufzeitprobleme haben die fachliche Bewertung zusaetzlich vernebelt, weil Startfehler und fehlende Qt-DLLs mit echten Testfehlern verwechselt werden konnten.

Fuer MAM gilt daher verbindlich: Ein Strompfadpfeil darf nicht selbst der Querverweis sein. Er ist nur die sichtbare Projektion eines fachlichen Fortsetzungspunktes. Der sichtbare Zieltext wird aus der Beziehung berechnet und darf nicht als primaere Beziehung gespeichert oder ausgewertet werden.

## Binding MAM Decisions For Strompfad Continuations

Diese Entscheidungen gelten fuer die naechste Umsetzung:

- Strompfade werden nicht frei geraten. Wenn bereits ein Strompfad angelegt ist, muss die Fortsetzung diesem Strompfad ueber eine Auswahl zugeordnet werden. Nach Nutzerwissen existiert eine solche Strompfad-/Pfad-Auswahl bzw. Pfadlogik bereits in QET und ist vor einer Neuanlage erneut zu pruefen.
- Bei mehreren gleichen Potential-/Signalfortsetzungen erfolgt die Zuordnung manuell ueber die vorhandene bzw. zu ergaenzende Auswahl. Automatische Kettenbildung aus Position, Seite oder Potentialname allein ist nicht akzeptiert.
- Automatik darf nur diagnostizieren und berechnen, nicht fachliche Kettenzuordnungen erfinden.
- Das verbindliche Standard-Anzeigeformat fuer den sichtbaren Zielverweis ist `/Seite.Spalte`, zum Beispiel `/2.4`.
- Seite und Spalte sind Zieladresse/Darstellung. Sie sind nicht die Identitaet des Potentials, Signals oder Strompfads.

## Suggested Implementation Slices

1. Fachmodell fuer Fortsetzungen festlegen:
   - Jede Potential-/Signalfortsetzung braucht eine stabile fachliche Identitaet.
   - Jede Fortsetzung verweist auf ein Potential oder Signal, nicht auf freien Text.
   - Richtung, Position, Seite, Raster/Pfad und sichtbarer Text bleiben getrennte Fakten.
   - Eine Kette braucht eine explizite manuelle Zuordnung ueber Strompfad-/Kettenauswahl und eine daraus bestimmbare Reihenfolge; ohne diese Information darf keine mehrgliedrige Kette geraten werden.
2. Read-only Continuation-Diagnose bauen:
   - Bestehende MAM-Pfeilelemente auswerten und als Fortsetzungs-Kandidaten projizieren.
   - Felder `potential`, `xref` und `voltage` nur als vorhandene Evidenz bzw. Darstellung lesen.
   - Fuer jeden Kandidaten Folio, Position, Richtung, Potential-/Signalname und sichtbaren Text ausgeben.
   - Status/Warnungen erzeugen: leeres Potential, fehlendes Gegenstueck, mehrdeutige Gegenstuecke, gleiche Richtung, widerspruechlicher sichtbarer `xref`, veralteter sichtbarer Text.
3. Sichere Querverweise ableiten:
   - Genau zwei passende Fortsetzungspunkte duerfen als 1:1-Querverweis projiziert werden.
   - Mehr als zwei gleiche Potential-/Signalfortsetzungen duerfen nur dann als Kette projiziert werden, wenn eine manuelle Strompfad-/Kettenzuordnung vorhanden ist.
   - Ohne eindeutige manuelle Zuordnung muss der Status `ambiguous continuation` entstehen.
4. Sichttext erst nach stabiler Projektion berechnen:
   - Der sichtbare Zieltext wird aus Zielseite und Zielspalte im Format `/Seite.Spalte` erzeugt.
   - Seiten-, Raster- oder Pfadverschiebungen duerfen nur den sichtbaren Zieltext aendern, nicht Potential-/Signalidentitaet oder Kettenzuordnung.
5. Schreib-/Synchronisationslogik erst danach:
   - Erst wenn die read-only Diagnose stabil ist, duerfen sichtbare `xref`-Texte aus berechneten Zieladressen aktualisiert werden.
   - Manuelle Aenderungen muessen entweder als fachliche Namensaenderung oder als Display-Override behandelt werden; beides darf nicht ununterscheidbar bleiben.
6. Persistiertes Fachobjekt einfuehren, sobald die Projektion bewiesen ist:
   - Fortsetzungspunkt, Potential/Signal, Ketten-ID, Reihenfolge, Richtung und Anzeigeformat muessen als getrennte Daten speicherbar werden.
   - UI/Navigator fuer Potential-/Signalfortsetzungen kommt erst nach dieser Fachmodellstabilisierung.
7. Grafische Darstellung aus dem Fachmodell generieren:
   - Pfeilform, Textposition, Farbe, Layer und Sichtbarkeit sind Darstellung.
   - Die Fachbeziehung wird nicht aus der Grafik rekonstruiert, sondern von ihr angezeigt.

## Public References

- IEC TC 3 document kinds and IEC 61082 overview: https://tc3.iec.ch/tc-activity/current_document_kinds/
- IEC 61175-1:2015 signal designations: https://webstore.iec.ch/en/publication/22490
- ISO/IEC 81346-1:2022 overview: https://www.iso.org/standard/82229.html
- DIN page for ISO/IEC 81346 reference designation aspects: https://www.din.de/de/referenzkennzeichnung-nach-der-normenreihe-iso-iec-81346-na-152-06-09-ga-695914
- EPLAN interruption point cross-reference glossary: https://www.eplan.help/en-us/Infoportal/Content/Plattform/2.9/Content/htm/Glossary_o_abbruchstellenquerverweise.htm
- EPLAN interruption point cross-references: https://eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/interruptionpointgui_k_darstellungabbruchstellen.htm
- EPLAN cross-references basics: https://eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/xessettingsgui_k_grundlagen.htm
- EPLAN potential definition point glossary: https://www.eplan.help/help/platform/2.9/en-us/help/Content/htm/Glossary_o_potenzialdefinitionspunkte.htm
- EPLAN potentials and signals: https://www.eplan.help/en-us/Infoportal/Content/Plattform/2022/Content/htm/potentialbrowsergui_k_start.htm
- EPLAN connection properties: https://eplan.help/en-us/Infoportal/Content/Plattform/2025/Content/htm/connectionbrowsergui_k_verbindungseigenschaften.htm
- WSCAD line plus node/arrow: https://docu.wscad.com/HELP/English/WSCAD54/AT/07_Schematic/07-01Connection_line/Line_%2B_Node_Arrow.htm
- WSCAD reference algorithm: https://docu.wscad.com/HELP/English/WSCAD54/AT/11_Automatic_function/11-03Querverweise/Reference_algorithm.htm
- Zuken E3.series sheet references / signal cross-references: https://www.zuken.com/doc/e3/series/en/Content/reference/dialogs/project_mode/reference_properties/IDD_DIALOG_SHEET_REFERENCE.htm
- Zuken E3.series connection target format: https://www.zuken.com/doc/e3/series/en/Content/reference/dialogs/project_mode/settings/settings_electric/connection_electric/IDD_OPTION_CONNECTION_TARGET_FORMAT_1.htm
- Zuken E3.series off-sheet connections article: https://www.zuken.com/us/blog/tech-tip-creating-off-sheet-connections-in-e3-series/
- AutoCAD Electrical signal arrows: https://help.autodesk.com/cloudhelp/2026/ENU/AutoCAD-Electrical/files/GUID-1A9583B1-BE8D-4823-BF01-AB6B081719C5.htm

# MAM Terminal Strips / Klemmenleisten Research

Date: 2026-09-29
Status: research and architecture decision basis for the private MAM fork

## Ziel

Die MAM-Klemmenarbeit darf nicht bei einer flachen CSV-Liste einzelner QET-Klemmen stehen bleiben. Die Nutzerreferenzen zeigen zwei gekoppelte Sichten:

- Klemmen sind im Stromlaufplan platzierte CAE-Objekte mit Leiter, Kabel/Ader, PE/Schirm und Gegenstelle.
- Dieselben Daten muessen als zusammenhaengende Klemmenleiste bzw. Klemmenleistenplan mit Mehrstockebenen und Bruecken ausgegeben werden.

Die zentrale fachliche Konsequenz ist: Eine Klemmenleiste ist eine geordnete Struktur aus physischen Klemmen, Ebenen/Anschluessen und Bruecken. Sie ist keine lose Sammlung einzelner Schaltplansymbole.

## Fachliche Grundbegriffe

- `Klemmenleiste / terminal strip`: geordnete Baugruppe auf der Tragschiene, z. B. `-X2.3`.
- `Physische Klemme / terminal block`: ein realer Klemmenartikel an einer Position der Leiste.
- `Mehrstockklemme / multi-level terminal`: eine physische Klemme mit mehreren Ebenen, z. B. `1L`, `1L0`, `1PE`.
- `Ebene / level / deck`: Stockwerk innerhalb einer physischen Klemme; Bruecken und Potentiale koennen ebenenbezogen sein.
- `Anschluss / connection point`: konkrete Leiter- oder Aderanschlussstelle einer Ebene.
- `Bruecke / jumper / cross-connection`: interne oder steckbare Verbindung zwischen Klemmenanschluessen; bei Mehrstockklemmen immer ebenen- bzw. funktionsbezogen zu betrachten.
- `PE`: Schutzleiterfunktion; in QET derzeit am ehesten ueber `terminal_type=ground` beobachtbar.
- `SH / Schirm`: in QET als Symbol-/Textkonvention vorhanden, aber nicht als eigener harter Terminaltyp.
- `Kabel/Ader`: Kabelkennung, Adernummer/-farbe, Querschnitt, Zielgeraet und Zielanschluss gehoeren zur sichtbaren CAE-Nutzbarkeit der Leiste.

## Herstellerkonzepte

### WAGO

WAGO beschreibt Reihenklemmen als breites System fuer Schaltschrankloesungen mit Durchgangs-, Funktions-, Doppelstock- und Dreistockklemmen. WAGO nennt explizit verschiedene Brueckmoeglichkeiten wie durchgehende, benachbarte, Reduzier-, Stufen- und Vertikalbruecker sowie Markier- und Pruefmoeglichkeiten. WAGO fuehrt Schirmanschlusstechnik separat als EMV-/Shield-Connection-System mit Schirmklemmbuegeln, Sammelschienenhaltern und Zubehoer.

Konsequenz fuer MAM:

- Mehrstock ist ein Artikel-/Konstruktionsmerkmal, nicht nur eine Darstellungsoption.
- Bruecken sind Zubehoer und Potentialverteilung; sie muessen als eigene Datenbeziehung sichtbar bleiben.
- Schirmanschluss ist fachlich relevant, aber nicht automatisch identisch mit einem normalen Leiterterminal.
- Markierung, Pruefpunkt und Zubehoer gehoeren spaeter an die physische Klemme bzw. Leiste.

Quelle: https://www.wago.com/global/c/rail-mount-terminal-blocks

### Phoenix Contact

Phoenix Contact CLIPLINE complete behandelt Klemmen als System aus Anschlussarten, Klemmenfamilien und standardisiertem Zubehoer. Die Produktuebersicht unterscheidet Durchgangs-/Mehrleiterklemmen, Mehrstockklemmen, Trenn-, Sicherungs-/Komponentenklemmen, Installationsklemmen, Sensor/Aktor-Klemmen und Schirmklemmen. Phoenix beschreibt Mehrstockklemmen als platzsparende Verbindung mehrerer Leiter ueber mehrere Ebenen; einzelne Potentiale koennen je Ebene gefuehrt werden. Brueckzubehoer umfasst u. a. steckbare Bruecken, Reduzierbruecken, Drahtbruecken und Potentialbruecken; Potentialbruecken sind fuer Doppelstock- und Mehrstockklemmen relevant und koennen Ebenen verbinden.

Konsequenz fuer MAM:

- Die Datenstruktur muss mehr als `terminal_label` speichern: Ebenen, Potentiale, Brueckenart, Zubehoer und Artikelbezug sind getrennte Dimensionen.
- Vertikale Bruecken zwischen Ebenen duerfen nicht mit horizontalen Bruecken entlang einer Ebene verwechselt werden.
- Dioden-, Sicherungs- und Trennklemmen sind Funktionsklemmen und muessen typisiert bleiben.

Quellen:

- https://www.phoenixcontact.com/assets/7412c45d-9275-46a5-8e65-168a1223de67/index.html
- https://www.phoenixcontact.com/en-us/products/multi-level-terminal-block-pt-25-3l-bu-3210509

### Weidmüller

Weidmueller/Klippon Connect beschreibt Durchgangsklemmen, 2-/3-/4-Leiterklemmen, Mehrleiter- und Mehrstockklemmen, Doppelstock- und Dreistockklemmen sowie PE-Klemmen. Weidmueller weist darauf hin, dass Anschlusslevel auf gleichem Potential liegen oder getrennt sein koennen. Das Portfolio enthaelt ausserdem PE-, Trenn-, Sicherungs- und Funktionsklemmen.

Konsequenz fuer MAM:

- `level_count` allein reicht nicht; jede Ebene braucht Funktion/Potentialtyp und Anschlussbezug.
- PE-Funktion ist ein Produkt- und Schutzleitermerkmal, nicht nur die Aderfarbe `GNYE`.
- Mehrleiterklemmen und Mehrstockklemmen sind zu unterscheiden: mehrere Leiter pro Potential ist nicht dasselbe wie mehrere Ebenen mit unterschiedlichen Potentialen.

Quelle: https://www.weidmueller.com/int/products/connectivity/terminal_blocks/feed_through_terminal_blocks.jsp

## CAE-Systemkonzepte

### EPLAN

EPLAN beschreibt Mehrstockklemmen als Klemmen mit mehreren uebereinander angeordneten Ebenen. Jede Klemme hat Level-Information; eine Mehrstockklemme kann aus verschiedenen Funktionen bestehen. Ebenen koennen eigene Stegbrueckenanschluesse haben, und fuer jede Ebene kann der Potentialtyp gesetzt werden. EPLAN erkennt mehrstockige Klemmen im Schaltplan ueber zusammengehoerige Terminal-Symbole mit gleicher Betriebsmittelkennzeichnung, Sortierung, Levelwerten und gleicher Geraeteposition. EPLANs allgemeine Terminal-Dokumentation nennt zudem Verwaltung von Klemmenleisten, Verdrahtungs- und Brueckenzielen sowie Mehrstockklemmen.

Konsequenz fuer MAM:

- Ein MAM-Service muss `device position / physical terminal position` und `level` als eigene Begriffe fuehren.
- Gleiches BMK allein reicht nicht; Sortierung und Level muessen zusammen die physische Klemme ergeben.
- Der Schaltplan zeigt normale Klemmsymbole, die CAE-Datenstruktur muss daraus die Mehrstockklemme rekonstruieren.

Quellen:

- https://eplan.help/en-us/infoportal/content/plattform/2022/content/htm/terminalgui_k_mehrstockklemmen.htm
- https://eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/terminalgui_k_start.htm
- https://eplan.help/en-us/Infoportal/Content/Plattform/2024/Content/htm/terminalgui_h_mehrstockklemmenarbeit.htm

### WSCAD

WSCAD beschreibt Multi-stage terminals im Terminal Browser. Dort werden Ebenen ueber ein Level-Register zusammengefuehrt; die Terminal Position Number zeigt, welche Anschluesse physisch zusammengehoeren. WSCAD erwaehnt ausserdem Sortierung nach Alphabet, Koordinate, Level oder gemischt; die definierte Reihenfolge wirkt auf Klemmenplan und Schaltschrankplatzierung.

Konsequenz fuer MAM:

- MAM braucht eine Terminal-Position-Nummer bzw. physische Positionsidentitaet, nicht nur eine sichtbare Klemmenbezeichnung.
- Der Klemmenplan muss dieselbe Reihenfolge verwenden wie die Leiste bzw. Schaltschrank-/Aufbauplanung.
- Mehrstocklogik ist eine Browser-/Manager-Funktion, nicht nur Symbolgrafik.

Quelle: https://docu.wscad.com/HELP/English/WSCAD54/AT/11_Automatic_function/11-17Klemmenplan/Klemmen_Browser/Multi-stage_in_the_terminal_browser.htm

### Zuken E3.series

Zuken E3.series dokumentiert das Zusammenfuehren einzelner Klemmen zu Mehrstockklemmen. Die Funktion `Merge terminals` ersetzt einzelne Klemmen durch eine Mehrstockklemme und verbindet Leiter/Draehte auf die neue Mehrstockklemme um. E3.series kennt ausserdem eine Terminal-Tabelle und nennt terminal plans als Teil der Schematic-Loesung; E3.cable kann einzelne Draehte zu mehradrigen Kabeln, Schirmen und verdrillten Paaren kombinieren.

Konsequenz fuer MAM:

- Der entscheidende CAE-Schritt ist nicht Export, sondern eine Operation, die Einzelklemmen in ein konsistentes Mehrstockgeraet zusammenfuehrt.
- Nach dem Zusammenfuehren muessen Leiter-/Kabelbeziehungen erhalten bleiben.
- Kabelschirm und Kabelstruktur sind in professionellen Systemen eigene Kabel-/Verdrahtungsdaten, nicht nur Freitext.

Quellen:

- https://www.zuken.com/doc/e3/series/en/Content/reference/functions/Terminal%20Plan/terminals/terminal_functions.htm
- https://www.zuken.com/en/product/e3series/

## Normen / Standards

Diese Recherche nutzt Normen nur als Orientierungsrahmen. Sie ersetzt keine Normenpruefung und zitiert keine vollstaendigen Normtexte.

- IEC 60947-7-1: Anforderungen an Klemmen fuer Kupferleiter fuer industrielle bzw. aehnliche Anwendungen. Relevant fuer Produktklassen, Leiterquerschnitte und Grundbegriff "terminal block".
- IEC 60947-7-2: Schutzleiterklemmen mit PE-/PEN-Funktion. Relevant fuer PE als Schutzleiterfunktion, nicht nur als Farbe.
- IEC 60947-7-3: Sicherungsklemmen. Relevant fuer Funktionsklemmen wie Sicherungs- und Komponentenklemmen.
- IEC 81346-1: Strukturierungsprinzipien und Referenzkennzeichnung technischer Objekte. Relevant fuer die Trennung von Funktions-, Orts- und Produktaspekt sowie eindeutige Betriebsmittelkennzeichen.
- IEC 61666 wird in Norm-/CAE-Kontexten fuer Terminalbezeichnungen referenziert; fuer MAM relevant ist insbesondere die Trennung von Betriebsmittel-/Klemmenleistenbezeichnung und konkreter Anschluss-/Terminalbezeichnung.

Quellen:

- https://webstore.iec.ch/en/publication/72949
- https://webstore.iec.ch/en/publication/4006
- https://www.vde-verlag.de/standards/0611015/din-en-60947-7-3-vde-0611-6-2010-05.html
- https://webstore.iec.ch/en/publication/64021

## Screenshot-Derived Requirements

Die Nutzerreferenzen zeigen mindestens zwei Zielbilder.

### Stromlaufplanintegration

Eine dreistoeckige Klemmenleiste wie `-X2.3` zeigt im Plan Klemmenbezeichnungen wie `1L`, `1L0`, `1PE`, `2L`, `2L0`, usw. Fachlich ist das eine physische Klemme `1` mit Ebenen `L`, `L0` und `PE`, nicht drei unabhaengige Klemmen. Kabeldaten wie `W1-50.15.01`, Kabeltyp, Aderzahl/Querschnitt, Laenge und Adern wie `1`, `2`, `3`, `GNYE` sind sichtbar mit den Anschluessen verbunden.

Anforderung:

- Parser/Service muss aus Bezeichnungen wie `1L`, `1L0`, `1PE` physische Position `1` und Ebene/Funktion ableiten koennen.
- PE/GNYE muss als Schutzleiter-Ebene erhalten bleiben.
- Kabel/Ader/Gegenstelle gehoeren an die jeweilige Ebene bzw. Anschlussbeziehung.
- Schirm/SH darf nicht erfunden werden; vorhandene QET-Schirmsymbole und Freitext-Hinweise muessen als Evidenz behandelt werden, bis ein hartes Modell existiert.

### Klemmenleistenplan

Der Klemmenleistenplan zeigt eine Matrix mit Zeilen wie `BRUECKE / JUMPERS`, `SYMBOL`, `KLEMMENNUMMER / TERMINAL NUMBER` und `ANSCHLUSS / CONNECTION`. Die Brueckenzeile zeigt Punkte und Linien ueber den jeweiligen Anschlussspalten.

Anforderung:

- Eine Ausgabe muss eine Matrix ueber `PhysicalTerminal` und `RealTerminal` bilden.
- Jede Spalte entspricht einem Anschluss/einer Ebene.
- Bruecken sind ebenenbezogene Spans/Gruppen, nicht pauschal Eigenschaften der ganzen physischen Klemme.
- Der spaetere grafische Plan muss aus derselben Matrix entstehen wie CSV/Regression.

## QET Ist-Zustand

QET enthaelt bereits wichtige Bausteine:

- `TerminalStrip` als projektweites Klemmenleistenobjekt.
- `PhysicalTerminal` als Gruppierung mehrerer `RealTerminal`.
- `RealTerminal` als Zuordnung zu einem platzierten Terminal-Element.
- `level` und `level_count` koennen Mehrstock abbilden.
- Bruecken existieren als `TerminalStripBridge` und werden nur fuer passende, zusammenhaengende Klemmen gleicher Ebene zugelassen.
- Terminaltypen existieren: `generic`, `fuse`, `sectional`, `diode`, `ground`.
- Terminalfunktionen existieren: `generic`, `phase`, `neutral`.
- Hersteller-/Artikelwerte koennen aus `Element::elementInformations()` gelesen werden.

Die Grenzen sind ebenso wichtig:

- Der QET-Erstellworkflow erzeugt keine zusammenhaengende Leiste aus dem Schaltplan; er sammelt freie Terminal-Elemente nachtraeglich.
- `PhysicalTerminal::uuid()` ist aktuell keine persistierte Projektidentitaet.
- Mehrstock entsteht manuell durch Gruppierung, nicht aus einem Artikel-/Klemmentyp-Profil.
- Schirm existiert in QET als Symbol-/Textkonvention, aber nicht als eigener Terminaltyp.
- Kabeltyp, Laenge, Aderstruktur und Schirm sind noch kein geschlossenes Kabelmodell innerhalb der MAM-Klemmenlogik.
- Klemmenplan-Layout existiert noch nicht als MAM-Matrixausgabe.

## MAM Zielarchitektur

MAM sollte die Klemmenleiste als fuehrende fachliche Struktur behandeln:

```text
TerminalStrip
  PhysicalTerminal
    Level / RealTerminal
      placed schematic terminal
      conductor / cable / core / counterpart
      bridge participation
```

Kernentscheidungen:

- `PhysicalTerminal` ist die physische Klemme bzw. Artikelposition.
- `RealTerminal` ist Ebene/Anschluss innerhalb dieser physischen Klemme.
- `level` darf nie flachgezogen werden.
- Bruecken haengen an RealTerminals/Ebenen und bilden Gruppen oder Spans.
- Herstellerartikel gehoeren fachlich an die physische Klemme, auch wenn QET sie heute oft am platzierten Symbol fuehrt.
- PE ist eine Schutzleiterfunktion; GNYE ist eine beobachtbare Ader-/Farbkonvention.
- SH/Schirm braucht ein eigenes Evidenzmodell oder spaeter ein hartes Semantikfeld.

## Implementierungs-Roadmap

1. Research dokumentieren und als Leitplanke fuer die Klemmenarbeit verwenden.
2. Normale 1-Stock-Klemmenleiste zuverlaessig aus vorhandenen Plan-Klemmen erzeugen: Labels `<prefix>:<number>` sammeln, Strip exakt nach `<prefix>` benennen, natuerlich numerisch sortieren, je Klemme ein `PhysicalTerminal` mit einem `RealTerminal`.
3. Read-only Terminal-Strip-Matrix aus `TerminalStrip -> PhysicalTerminal -> RealTerminal` ableiten, sobald die einfache Leiste stabil erstellbar ist.
4. Coherent Multilevel Strip Assignment: aus vorhandenen Plan-Klemmen wie `1L`, `1L0`, `1PE` eine zusammenhaengende Mehrstock-Klemmenleiste rekonstruieren.
5. Persistente `PhysicalTerminal`-Identitaet pruefen und einfuehren, wenn der Matrix-/Assignment-Slice die Notwendigkeit bestaetigt.
6. Bulk-Erzeugung "Neue Klemmenleiste" mit Anzahl, Ebenen, PE/SH, Typ und Artikelprofil.
7. Grafischer MAM-Klemmenleistenplan aus der Matrix: Brueckenzeile, Symbolzeile, Klemmennummer, Anschluss/Gegenstelle, Kabel/Ader.
8. Hersteller-/Artikelprofile fuer WAGO, Phoenix Contact und Weidmueller ausbauen.

## Quellen

- WAGO Rail-Mount Terminal Blocks: https://www.wago.com/global/c/rail-mount-terminal-blocks
- Phoenix Contact terminal blocks / CLIPLINE complete: https://www.phoenixcontact.com/assets/7412c45d-9275-46a5-8e65-168a1223de67/index.html
- Phoenix Contact PT 2,5-3L product example: https://www.phoenixcontact.com/en-us/products/multi-level-terminal-block-pt-25-3l-bu-3210509
- Weidmueller feed-through and PE terminal blocks: https://www.weidmueller.com/int/products/connectivity/terminal_blocks/feed_through_terminal_blocks.jsp
- EPLAN Multi-level Terminals: https://eplan.help/en-us/infoportal/content/plattform/2022/content/htm/terminalgui_k_mehrstockklemmen.htm
- EPLAN Terminals: https://eplan.help/en-us/Infoportal/Content/Plattform/2027/Content/htm/terminalgui_k_start.htm
- EPLAN Defining Multi-level Terminals: https://eplan.help/en-us/Infoportal/Content/Plattform/2024/Content/htm/terminalgui_h_mehrstockklemmenarbeit.htm
- WSCAD Multi-stage terminals: https://docu.wscad.com/HELP/English/WSCAD54/AT/11_Automatic_function/11-17Klemmenplan/Klemmen_Browser/Multi-stage_in_the_terminal_browser.htm
- Zuken E3.series terminal functions: https://www.zuken.com/doc/e3/series/en/Content/reference/functions/Terminal%20Plan/terminals/terminal_functions.htm
- Zuken E3.series product overview: https://www.zuken.com/en/product/e3series/
- IEC 60947-7-1: https://webstore.iec.ch/en/publication/72949
- IEC 60947-7-2: https://webstore.iec.ch/en/publication/4006
- DIN EN 60947-7-3 / VDE 0611-6: https://www.vde-verlag.de/standards/0611015/din-en-60947-7-3-vde-0611-6-2010-05.html
- IEC 81346-1: https://webstore.iec.ch/en/publication/64021

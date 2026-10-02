# Roadmap: Potentiale, Fortsetzungen und Verweise

Date: 2026-10-02
Status: binding working roadmap for the private MAM potential/continuation/reference track

## Ziel

MAM soll Potentiale, Signale, Leiter, Fortsetzungspunkte und Querverweise als getrennte fachliche Fakten fuehren. Sichtbare Pfeile und Texte sind nur Darstellung bzw. Projektion dieser Fakten.

Der Fahrplan ersetzt keine Norm-/CAE-Analyse. Er uebersetzt die bestehenden Analysen in umsetzbare, pruefbare Arbeitsschnitte.

## Aktueller Stand

Bereits vorhanden:

- `--export-mam-terminal-potential` liest Leiter, Endpunkte, Potentialgruppen, Raster-/Pfadwerte und Blatt-/Pfadreferenzen read-only.
- `CrossReferenceProjectionService` sammelt Contact-, PLC-, Report/Folio-, Terminal-, Cable/Core- und jetzt auch Potential-Continuation-Verweise als read-only Backbone.
- `--export-mam-continuation` diagnostiziert bestehende MAM-Potentialpfeile read-only.
- `ContinuationProjectionService` erkennt bestehende `mam_potential_*`-Pilotpfeile und meldet:
  - Potential/Signal, Spannung, Richtung, Folio, Raster/Pfad und sichtbaren `xref`;
  - berechneten Zielverweis;
  - `OK`, `WARNING` oder `ERROR`;
  - veraltete sichtbare Verweise;
  - fehlende Gegenstuecke;
  - widerspruechliche oder nicht eindeutige Fortsetzungen.
- Die erste Diagnose-Haertung ist umgesetzt und fokussiert getestet:
  - genau zwei gleiche Potentialpfeile als 1:1-Verweis;
  - fehlendes Gegenstueck;
  - leerer Potential-/Signalname;
  - veralteter sichtbarer `xref`;
  - mehr als zwei gleiche Potentiale ohne Ketteninformation;
  - gleiche Richtung bei einem 1:1-Paar;
  - `potential_continuation`-Zeilen im allgemeinen CrossReference-Backbone.
- Die erste Ketten-Diagnose ist ebenfalls read-only getestet:
  - geordnete `chain`/`chain_order`-Kette ohne Warnung;
  - fehlende `chain_order` als Warnung;
  - doppelte `chain_order` als Warnung;
  - fehlendes Ziel am Kettenanfang/-ende als Fehler.
- `--export-mam-cross-reference` zaehlt `WARNING` und `ERROR` jetzt getrennt und stuft Continuation-Fehler nicht mehr zu Warnungen herunter.

Wichtig: Die aktuellen Pilotpfeile mit `potential`, `xref` und `voltage` sind weiterhin keine akzeptierte Fachlogik. Sie sind nur Eingabeevidenz fuer die Diagnose.

## Verbindliche Reihenfolge

1. Read-only Fakten sicher machen.
2. Diagnose und Verweise stabilisieren.
3. Persistierte Fachidentitaet fuer Fortsetzungen einfuehren.
4. Sichtbare Texte aus der Fachbeziehung berechnen.
5. Bedienlogik und automatische Aktualisierung erst danach.

Keine UI-Synchronisation, kein Write-back und keine XML-/Core-Migration, bevor die Diagnose ueber repräsentative Beispiele stabil ist.

## Phase 1: Diagnose stabilisieren

Ziel: Alle bestehenden Potential-/Fortsetzungspfeile werden gefunden und nie still uebersprungen.

Aufgaben:

- `--export-mam-continuation` als primaere Pruefflaeche beibehalten. Status: erster Slice umgesetzt.
- Diagnosemeldungen erweitern und stabil benennen. Status: folgende Meldungen sind im ersten Slice abgedeckt:
  - `ERROR: empty potential or signal`
  - `ERROR: missing continuation counterpart`
  - `ERROR: contradictory continuation direction`
  - `WARNING: ambiguous continuation group without chain order`
  - `WARNING: stale visible xref ...`
  - `WARNING: empty visible xref`
- Fixture-/Testabdeckung fuer mindestens diese Faelle schaffen. Status: erster Slice umgesetzt fuer:
  - genau zwei gleiche Potentialpfeile als 1:1-Verweis;
  - fehlendes Gegenstueck;
  - leerer Potential-/Signalname;
  - veralteter sichtbarer `xref`;
  - mehr als zwei gleiche Potentiale ohne Ketteninformation;
  - gleiche Richtung bei einem 1:1-Paar.
- CrossReference-Export pruefen: `potential_continuation`-Zeilen muessen dieselben Status- und Diagnosefakten tragen. Status: umgesetzt und fokussiert getestet.
- Kettenlogik ist gezielt getestet, bevor sie persistiert wird:
  - eindeutige Kette mit Reihenfolge als OK;
  - fehlende `chain_order` als Warnung;
  - doppelte `chain_order` als Warnung;
  - fehlendes Ziel innerhalb einer Kette als Fehler.

Akzeptanz:

- Ein fehlerhafter Strompfadversuch bricht nicht still ab.
- Jeder erkannte Pfeil erzeugt eine CSV-Zeile.
- Unklare Faelle werden als `WARNING`/`ERROR` sichtbar, nicht geraten.

## Phase 2: Fortsetzungs-Fachmodell definieren

Ziel: Die Diagnose benennt exakt, welche fachlichen Daten fehlen, bevor sie persistiert werden.

Zu definierende Fachobjekte:

- `Potential` oder `Signal` als fachlicher Bezug.
- `Continuation` mit stabiler ID, Folio, Position, Richtung und Anzeigeformat.
- `ContinuationGroup` bzw. Kette mit stabiler ID.
- Reihenfolge innerhalb einer Kette.
- Optional explizite Paarung fuer manuell gesetzte Point-to-Point-Faelle.
- Berechneter `CrossReference` als abgeleitete Beziehung, nicht als gespeicherter sichtbarer Text.

Offene Entscheidungen:

- Soll die erste persistierte Beziehung `chain`/`chain_order`, explizite Paar-ID oder beides erlauben?
- Wird ein Potentialname zunaechst als String gefuehrt oder direkt auf ein spaeteres Potentialobjekt abgebildet?
- Welche vorhandene QET-Pfad-/Strompfad-Auswahl kann wiederverwendet werden?
- Wie werden vorhandene Pilotpfeile migriert oder als Alt-Evidenz behandelt?

Akzeptanz:

- Mehr als zwei gleichnamige Fortsetzungen koennen ohne Raten einer Kette oder einem Warnstatus zugeordnet werden.
- Seite/Pfad/Raster bleiben Zieladresse, nicht Identitaet.
- `xref` bleibt Display, nicht Beziehung.

## Phase 3: Persistenz einfuehren

Ziel: Fortsetzungen werden als fachliche Projektinformationen gespeichert, ohne die sichtbaren Texte zur Wahrheit zu machen.

Aufgaben:

- Minimalen XML-/Projekt-Speicherort fuer MAM-spezifische Continuation-Fakten festlegen.
- Load/Save-Roundtrip fuer Continuation-ID, Potential/Signal-Bezug, Richtung, Kette und Reihenfolge testen.
- Alte Pilotpfeile weiter lesbar lassen.
- Diagnose so erweitern, dass sie zwischen persistierter Fachbeziehung und sichtbarem Pilotfeld unterscheidet.

Akzeptanz:

- Resave veraendert Continuation-Beziehungen nicht.
- Umbenennen/Verschieben von Seiten aendert nicht die fachliche Identitaet.
- Sichtbarer `xref` kann als veraltet erkannt werden.

## Phase 4: Sichttext berechnen

Ziel: Der sichtbare Pfeiltext wird aus der stabilen Beziehung berechnet.

Aufgaben:

- Standardformat `/Seite.Pfad` als Default verwenden.
- Anzeigeoptionen spaeter konfigurierbar halten, aber nicht als erste Abhaengigkeit bauen.
- Read-only Vergleich zwischen `visible_xref` und `computed_xref` beibehalten.
- Erst nach stabiler Diagnose eine Sync-Funktion fuer sichtbare `xref`-Felder bauen.

Akzeptanz:

- Nach Seiten-/Pfadverschiebung aendert sich nur der berechnete Zieltext.
- Potential-/Signalidentitaet und Kettenzuordnung bleiben unveraendert.
- Veraltete Texte werden vor Write-back klar gemeldet.

## Phase 5: Bedienlogik

Ziel: Benutzer koennen Fortsetzungen fachlich setzen, statt freie Pfeiltexte zu pflegen.

Aufgaben:

- Pfeil setzen.
- Potential oder Signal auswaehlen.
- Kette auswaehlen oder erzeugen.
- Reihenfolge innerhalb der Kette setzen/aendern.
- Warnungen direkt im Plan und/oder Report sichtbar machen.
- Sichttext aus berechnetem Verweis aktualisieren.

Akzeptanz:

- Der Benutzer muss bei mehr als zwei gleichen Potentialen nicht aus impliziten Positionen raten lassen.
- MAM meldet Ambiguitaet, bevor es falsche Verweise schreibt.
- Manuelle Aenderungen werden eindeutig als Fachwertaenderung oder Display-Override behandelt.

## Naechster empfohlener Slice

Der naechste kleine Umsetzungsschritt ist weiterhin nicht UI und nicht Write-back, sondern die Fachmodellentscheidung vor Persistenz:

1. Entscheiden, ob die erste persistierte Zusatzinformation `chain`/`chain_order`, explizite Paar-ID oder beides wird.
2. Falls `chain`/`chain_order` akzeptiert wird, einen minimalen Load/Save-Ort fuer diese Fakten definieren.
3. Falls explizite Paar-ID erforderlich ist, zuerst deren 1:1-Diagnose in `--export-mam-continuation` ergaenzen.
4. Erst danach Phase 2/3 beginnen.

Erst wenn dieser Slice stabil ist, beginnt Phase 2/3 mit persistierten Fachobjekten.

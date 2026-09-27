# MAM CAE-Bibliothek: Schütze und Relais

Importierbare QElectroTech-Sammlung mit Spulen, Gerätekontaktvorräten für den
Kontaktspiegel und dazugehörigen Einzelkontakten zur Verknüpfung im Plan.

## Enthaltene Kontaktbestückungen

- Hilfsschütz 40E: DILA/DILER-40-Muster, 4 Schließer, A1/A2, 13-14, 23-24,
  33-34, 43-44.
- Hilfsschütz 31E: DILA/DILER-31-Muster, 3 Schließer + 1 Öffner, A1/A2,
  13-14, 21-22, 33-34, 43-44.
- Hilfsschütz 22E: DILA/DILER-22-Muster, 2 Schließer + 2 Öffner, A1/A2,
  13-14, 21-22, 31-32, 43-44.
- Vormontierte Erweiterungsprofile aus der Eaton-Abbildung 4-3: DILA-40E +
  XHI04, DILA-31 + XHI13 und DILA-22 + XHI22. Alle drei ergeben 4 Schließer
  und 4 Öffner. Die Zusatzkontakte und ihre Klemmenbezeichnungen sind im
  jeweiligen Spulenelement enthalten.
- Koppelrelais 2 Wechsler: herstellerneutrales Grundmuster mit 11-12-14 und
  21-22-24.
- Leistungsschütz 3 Hauptschließer + 1 Hilfsschließer + 1 Hilfsöffner:
  herstellerneutrales Grundmuster mit 1-2, 3-4, 5-6, 13-14 und 21-22.
- Einzelne QET-Master/Slave-Kontakte für Schließer und Öffner.

Die Kontaktbestückungen und Kombinationen orientieren sich an der Kontaktlage
und den Anschlussbezeichnungen im Eaton-Schaltungsbuch 10/23, Kapitel
„Hilfsschütze“, Seiten 4-2 und 4-3. Die Bestückung der drei kombinierten
Profile ist dort als DILA-40/04, DILA-31/13 und DILA-22/22 gezeigt. Die
Bibliothek bildet diese Eaton-Funktionskombinationen ab, aber noch keine
vollständigen Bestellartikel mit Spulenspannung, Schaltvermögen und weiterem
Zubehör.

Spulen deklarieren die gesamte vorgesehene Kontaktbestückung, damit der
Kontaktspiegel die NO-/NC-Kontakte mit Klemmenbezeichnungen unterhalb der
Spule anzeigen kann. Einzelne Kontakte werden im Schaltplan als Slaves mit der
passenden Spule (Master) verbunden.

Für die drei Eaton-Erweiterungskombinationen muss derzeit das passende
„Basisgerät + XHI“-Spulenelement ausgewählt werden. Ein Zusatzblock, der nach
dem Platzieren an eine vorhandene Spule angefügt wird, erweitert deren
Kontaktspiegel in dieser Version **noch nicht automatisch**. Dafür fehlt
QElectroTech derzeit die Geräte-/Zubehör-Zusammensetzung; sie ist der nächste
notwendige Software-Schritt.

## Installation

In QElectroTech unter **Einstellungen → Elemente → Benutzerdefinierte Sammlung**
den Ordner `mam_cae_library` auswählen. Alternativ dessen Unterordner und
Elementdateien in die konfigurierte Benutzer-Sammlung kopieren und die
Elementleiste aktualisieren. Die eingebundene QET-Standardsammlung bleibt
unverändert.

/*
	Copyright 2006-2026 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech. If not, see <http://www.gnu.org/licenses/>.
*/
#include <QtTest>

#include "cli_test_utils.h"

#include <QFile>
#include <QTemporaryDir>

namespace {

QHash<QString, QString> rowByTerminalUuid(const QList<QStringList> &rows, const QString &terminal_uuid)
{
	QHash<QString, QString> values;
	if (rows.isEmpty())
		return values;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("terminal_uuid"))) != terminal_uuid)
			continue;

		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		return values;
	}
	return values;
}

} // namespace

class tst_mam_terminal_strip_export : public QObject
{
	Q_OBJECT

private slots:
	void exportsReadOnlyMamTerminalStripCsv()
	{
		const QString fixture = QFINDTESTDATA("fixtures/terminal_strip_synthetic_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "synthetic terminal-strip fixture project not found");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString export_path = dir.filePath(QStringLiteral("mam_terminal_strip.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-terminal-strip"),
			fixture,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 2 MAM TerminalStrip terminal(s), 0 warning row(s)")));
		QVERIFY2(QFile::exists(export_path), "terminal-strip export output was not created");

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 3);
		QCOMPARE(
			rows.first(),
			QStringList({
				QStringLiteral("strip_installation"),
				QStringLiteral("strip_location"),
				QStringLiteral("strip_name"),
				QStringLiteral("strip_uuid"),
				QStringLiteral("physical_index"),
				QStringLiteral("level"),
				QStringLiteral("level_count"),
				QStringLiteral("terminal_label"),
				QStringLiteral("terminal_uuid"),
				QStringLiteral("terminal_folio"),
				QStringLiteral("terminal_xref"),
				QStringLiteral("terminal_name"),
				QStringLiteral("conductor"),
				QStringLiteral("bridge_uuid"),
				QStringLiteral("bridge_color"),
				QStringLiteral("status"),
				QStringLiteral("warnings")
			}));

		const QHash<QString, QString> first = rowByTerminalUuid(
			rows,
			QStringLiteral("aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa1"));
		const QHash<QString, QString> second = rowByTerminalUuid(
			rows,
			QStringLiteral("aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa2"));
		QVERIFY2(!first.isEmpty(), "first synthetic terminal row missing");
		QVERIFY2(!second.isEmpty(), "second synthetic terminal row missing");

		for (const QHash<QString, QString> &row : {first, second}) {
			QCOMPARE(row.value(QStringLiteral("strip_installation")), QStringLiteral("=SYN"));
			QCOMPARE(row.value(QStringLiteral("strip_location")), QStringLiteral("+QA"));
			QCOMPARE(row.value(QStringLiteral("strip_name")), QStringLiteral("XT-SYN"));
			QCOMPARE(row.value(QStringLiteral("strip_uuid")), QStringLiteral("bbbbbbbb-bbbb-4bbb-8bbb-bbbbbbbbbbbb"));
			QCOMPARE(row.value(QStringLiteral("level")), QStringLiteral("1"));
			QCOMPARE(row.value(QStringLiteral("level_count")), QStringLiteral("1"));
			QCOMPARE(row.value(QStringLiteral("terminal_folio")), QStringLiteral("1"));
			QCOMPARE(row.value(QStringLiteral("terminal_name")), QStringLiteral("Synthetic terminal block"));
			QCOMPARE(row.value(QStringLiteral("bridge_uuid")), QStringLiteral("cccccccc-cccc-4ccc-8ccc-cccccccccccc"));
			QCOMPARE(row.value(QStringLiteral("bridge_color")), QStringLiteral("#ff8800"));
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("OK"));
			QCOMPARE(row.value(QStringLiteral("warnings")), QString());
		}

		QCOMPARE(first.value(QStringLiteral("physical_index")), QStringLiteral("1"));
		QCOMPARE(second.value(QStringLiteral("physical_index")), QStringLiteral("2"));
	}
};

QTEST_APPLESS_MAIN(tst_mam_terminal_strip_export)

#include "tst_mam_terminal_strip_export.moc"

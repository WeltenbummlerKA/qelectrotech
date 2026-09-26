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

#include <QTemporaryDir>

namespace {

QHash<QString, QString> rowByWireNumber(const QList<QStringList> &rows, const QString &wire_number)
{
	QHash<QString, QString> values;
	if (rows.isEmpty())
		return values;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("wire_number"))) != wire_number)
			continue;

		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		return values;
	}
	return values;
}

} // namespace

class tst_mam_terminal_potential_export : public QObject
{
	Q_OBJECT

private slots:
	void exportsReadOnlyMamTerminalPotentialCsv()
	{
		const QString fixture = QFINDTESTDATA("fixtures/workflow_exports_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "terminal/potential fixture project not found");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString export_path = dir.filePath(QStringLiteral("mam_terminal_potential.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-terminal-potential"),
			fixture,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 7 MAM Terminal/Potential conductor(s), 0 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 8);
		QCOMPARE(
			rows.first(),
			QStringList({
				QStringLiteral("wire_number"),
				QStringLiteral("conductor_uuid"),
				QStringLiteral("folio"),
				QStringLiteral("from_element_label"),
				QStringLiteral("from_element_uuid"),
				QStringLiteral("from_terminal"),
				QStringLiteral("to_element_label"),
				QStringLiteral("to_element_uuid"),
				QStringLiteral("to_terminal"),
				QStringLiteral("potential_wire_number"),
				QStringLiteral("potential_conductor_count"),
				QStringLiteral("potential_terminal_count"),
				QStringLiteral("status"),
				QStringLiteral("warnings")
			}));

		const QSet<QString> expected_wires {
			QStringLiteral("W005"),
			QStringLiteral("W006"),
			QStringLiteral("W007"),
			QStringLiteral("W008"),
			QStringLiteral("W009"),
			QStringLiteral("W010"),
			QStringLiteral("W011")
		};
		QCOMPARE(CliTestUtils::columnSet(rows, QStringLiteral("wire_number")), expected_wires);

		for (const QString &wire : expected_wires) {
			const QHash<QString, QString> row = rowByWireNumber(rows, wire);
			QVERIFY2(!row.isEmpty(), qPrintable(QStringLiteral("missing row for wire '%1'").arg(wire)));
			QCOMPARE(row.value(QStringLiteral("folio")), QStringLiteral("1"));
			QVERIFY2(!row.value(QStringLiteral("conductor_uuid")).isEmpty(),
					 qPrintable(QStringLiteral("empty conductor uuid for wire '%1'").arg(wire)));
			QVERIFY2(!row.value(QStringLiteral("from_element_uuid")).isEmpty(),
					 qPrintable(QStringLiteral("empty from element uuid for wire '%1'").arg(wire)));
			QVERIFY2(!row.value(QStringLiteral("to_element_uuid")).isEmpty(),
					 qPrintable(QStringLiteral("empty to element uuid for wire '%1'").arg(wire)));
			QCOMPARE(row.value(QStringLiteral("potential_wire_number")), wire);
			QCOMPARE(row.value(QStringLiteral("potential_conductor_count")), QStringLiteral("1"));
			QCOMPARE(row.value(QStringLiteral("potential_terminal_count")), QStringLiteral("2"));
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("OK"));
			QCOMPARE(row.value(QStringLiteral("warnings")), QString());
		}

		const QHash<QString, QString> w005 = rowByWireNumber(rows, QStringLiteral("W005"));
		QVERIFY(!w005.isEmpty());
		QCOMPARE(w005.value(QStringLiteral("from_element_label")), QStringLiteral("v2_transformer_2w_primary-delta_secondary-wye_g"));
		QCOMPARE(w005.value(QStringLiteral("from_terminal")), QStringLiteral("bottom"));
		QCOMPARE(w005.value(QStringLiteral("to_element_label")), QStringLiteral("v2_circuit_breaker"));
		QCOMPARE(w005.value(QStringLiteral("to_terminal")), QStringLiteral("top"));
	}
};

QTEST_APPLESS_MAIN(tst_mam_terminal_potential_export)

#include "tst_mam_terminal_potential_export.moc"

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

QHash<QString, QString> rowByEndpoints(
	const QList<QStringList> &rows,
	const QString &from_element_label,
	const QString &to_element_label)
{
	QHash<QString, QString> values;
	if (rows.isEmpty())
		return values;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("from_element_label"))) != from_element_label)
			continue;
		if (fields.value(header.indexOf(QStringLiteral("to_element_label"))) != to_element_label)
			continue;

		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		return values;
	}
	return values;
}

QString writeFixtureVariant(QTemporaryDir &dir, const QString &file_name, const QString &content)
{
	const QString path = dir.filePath(file_name);
	QFile file(path);
	if (!dir.isValid() || !file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		return {};
	if (file.write(content.toUtf8()) <= 0)
		return {};
	file.close();
	return path;
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
				QStringLiteral("from_grid"),
				QStringLiteral("to_grid"),
				QStringLiteral("grid_range"),
				QStringLiteral("from_path"),
				QStringLiteral("to_path"),
				QStringLiteral("path_range"),
				QStringLiteral("from_reference"),
				QStringLiteral("to_reference"),
				QStringLiteral("reference_range"),
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
			QVERIFY2(!row.value(QStringLiteral("from_grid")).isEmpty(),
					 qPrintable(QStringLiteral("empty from grid for wire '%1'").arg(wire)));
			QVERIFY2(!row.value(QStringLiteral("to_grid")).isEmpty(),
					 qPrintable(QStringLiteral("empty to grid for wire '%1'").arg(wire)));
			QVERIFY2(row.value(QStringLiteral("from_grid")) != QStringLiteral("-"),
					 qPrintable(QStringLiteral("out-of-bounds from grid for wire '%1'").arg(wire)));
			QVERIFY2(row.value(QStringLiteral("to_grid")) != QStringLiteral("-"),
					 qPrintable(QStringLiteral("out-of-bounds to grid for wire '%1'").arg(wire)));
			QVERIFY2(!row.value(QStringLiteral("from_path")).isEmpty(),
					 qPrintable(QStringLiteral("empty from path for wire '%1'").arg(wire)));
			QVERIFY2(!row.value(QStringLiteral("to_path")).isEmpty(),
					 qPrintable(QStringLiteral("empty to path for wire '%1'").arg(wire)));
			QCOMPARE(
				row.value(QStringLiteral("path_range")),
				row.value(QStringLiteral("from_path")) == row.value(QStringLiteral("to_path"))
					? row.value(QStringLiteral("from_path"))
					: row.value(QStringLiteral("from_path")) % QStringLiteral(" -> ")
						% row.value(QStringLiteral("to_path")));
			QCOMPARE(row.value(QStringLiteral("from_reference")),
					 QStringLiteral("/1.") % row.value(QStringLiteral("from_path")));
			QCOMPARE(row.value(QStringLiteral("to_reference")),
					 QStringLiteral("/1.") % row.value(QStringLiteral("to_path")));
			QCOMPARE(
				row.value(QStringLiteral("reference_range")),
				row.value(QStringLiteral("from_reference")) == row.value(QStringLiteral("to_reference"))
					? row.value(QStringLiteral("from_reference"))
					: row.value(QStringLiteral("from_reference")) % QStringLiteral(" -> ")
						% row.value(QStringLiteral("to_reference")));
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

	void exportsExistingTerminalPotentialWarnings()
	{
		const QString fixture = QFINDTESTDATA("fixtures/workflow_exports_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "terminal/potential fixture project not found");

		QString xml = QString::fromUtf8(CliTestUtils::readFile(fixture));
		QVERIFY2(!xml.isEmpty(), "fixture content not readable");
		const QString wire = QStringLiteral("num=\"W005\"");
		QVERIFY2(xml.contains(wire), "expected W005 conductor XML not found in fixture");
		xml.replace(wire, QStringLiteral("num=\"\""));

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString variant = writeFixtureVariant(
			dir,
			QStringLiteral("empty_terminal_potential_wire_number.qet"),
			xml);
		QVERIFY2(!variant.isEmpty(), "empty wire-number variant write failed");

		const QString export_path = dir.filePath(QStringLiteral("mam_terminal_potential_warnings.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-terminal-potential"),
			variant,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 7 MAM Terminal/Potential conductor(s), 1 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 8);

		const QHash<QString, QString> warning_row = rowByEndpoints(
			rows,
			QStringLiteral("v2_transformer_2w_primary-delta_secondary-wye_g"),
			QStringLiteral("v2_circuit_breaker"));
		QVERIFY(!warning_row.isEmpty());
		QCOMPARE(warning_row.value(QStringLiteral("wire_number")), QString());
		QVERIFY2(!warning_row.value(QStringLiteral("conductor_uuid")).isEmpty(), "warning row should keep conductor identity");
		QCOMPARE(warning_row.value(QStringLiteral("potential_wire_number")), QString());
		QCOMPARE(warning_row.value(QStringLiteral("potential_conductor_count")), QStringLiteral("1"));
		QCOMPARE(warning_row.value(QStringLiteral("potential_terminal_count")), QStringLiteral("2"));
		QCOMPARE(warning_row.value(QStringLiteral("status")), QStringLiteral("WARNING"));
		QCOMPARE(warning_row.value(QStringLiteral("warnings")), QStringLiteral("empty potential wire number"));
	}
};

QTEST_APPLESS_MAIN(tst_mam_terminal_potential_export)

#include "tst_mam_terminal_potential_export.moc"

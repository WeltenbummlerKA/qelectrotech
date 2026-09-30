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
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 4 MAM TerminalStrip terminal(s), 0 warning row(s)")));
		QVERIFY2(QFile::exists(export_path), "terminal-strip export output was not created");

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 5);
		QCOMPARE(
			rows.first(),
			QStringList({
				QStringLiteral("strip_installation"),
				QStringLiteral("strip_location"),
				QStringLiteral("strip_name"),
				QStringLiteral("strip_uuid"),
				QStringLiteral("physical_terminal_key"),
				QStringLiteral("physical_index"),
				QStringLiteral("level"),
				QStringLiteral("level_count"),
				QStringLiteral("terminal_label"),
				QStringLiteral("terminal_uuid"),
				QStringLiteral("terminal_folio"),
				QStringLiteral("terminal_xref"),
				QStringLiteral("terminal_name"),
				QStringLiteral("terminal_type"),
				QStringLiteral("terminal_function"),
				QStringLiteral("terminal_manufacturer"),
				QStringLiteral("terminal_designation"),
				QStringLiteral("terminal_description"),
				QStringLiteral("terminal_led"),
				QStringLiteral("terminal_is_multilevel"),
				QStringLiteral("terminal_connection_point_count"),
				QStringLiteral("terminal_connection_points"),
				QStringLiteral("connection_side_count"),
				QStringLiteral("connection_sides"),
				QStringLiteral("connection_side_conductor_uuids"),
				QStringLiteral("connection_side_conductors"),
				QStringLiteral("connection_side_counterpart_elements"),
				QStringLiteral("connection_side_counterpart_element_uuids"),
				QStringLiteral("connection_side_counterpart_terminals"),
				QStringLiteral("connection_side_counterpart_folios"),
				QStringLiteral("conductor"),
				QStringLiteral("connection_count"),
				QStringLiteral("connected_terminals"),
				QStringLiteral("connected_conductor_uuids"),
				QStringLiteral("connected_conductors"),
				QStringLiteral("connected_cables"),
				QStringLiteral("connected_wire_colors"),
				QStringLiteral("connected_wire_sections"),
				QStringLiteral("connected_conductor_functions"),
				QStringLiteral("counterpart_elements"),
				QStringLiteral("counterpart_element_uuids"),
				QStringLiteral("counterpart_terminals"),
				QStringLiteral("counterpart_folios"),
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
		const QHash<QString, QString> multilevel_first = rowByTerminalUuid(
			rows,
			QStringLiteral("aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa3"));
		const QHash<QString, QString> multilevel_second = rowByTerminalUuid(
			rows,
			QStringLiteral("aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa4"));
		QVERIFY2(!first.isEmpty(), "first synthetic terminal row missing");
		QVERIFY2(!second.isEmpty(), "second synthetic terminal row missing");
		QVERIFY2(!multilevel_first.isEmpty(), "first multilevel synthetic terminal row missing");
		QVERIFY2(!multilevel_second.isEmpty(), "second multilevel synthetic terminal row missing");

		for (const QHash<QString, QString> &row : {first, second}) {
			QCOMPARE(row.value(QStringLiteral("strip_installation")), QStringLiteral("=SYN"));
			QCOMPARE(row.value(QStringLiteral("strip_location")), QStringLiteral("+QA"));
			QCOMPARE(row.value(QStringLiteral("strip_name")), QStringLiteral("XT-SYN"));
			QCOMPARE(row.value(QStringLiteral("strip_uuid")), QStringLiteral("bbbbbbbb-bbbb-4bbb-8bbb-bbbbbbbbbbbb"));
			QCOMPARE(row.value(QStringLiteral("level")), QStringLiteral("1"));
			QCOMPARE(row.value(QStringLiteral("level_count")), QStringLiteral("1"));
			QCOMPARE(row.value(QStringLiteral("terminal_folio")), QStringLiteral("1"));
			QCOMPARE(row.value(QStringLiteral("terminal_name")), QStringLiteral("Synthetic terminal block"));
			QCOMPARE(row.value(QStringLiteral("terminal_type")), QStringLiteral("generic"));
			QCOMPARE(row.value(QStringLiteral("terminal_function")), QStringLiteral("generic"));
			QCOMPARE(row.value(QStringLiteral("terminal_is_multilevel")), QStringLiteral("false"));
			QCOMPARE(row.value(QStringLiteral("terminal_connection_point_count")), QStringLiteral("3"));
			QCOMPARE(row.value(QStringLiteral("terminal_connection_points")), QStringLiteral("bottom | side | top"));
			QCOMPARE(row.value(QStringLiteral("connection_side_count")), QStringLiteral("3"));
			QCOMPARE(row.value(QStringLiteral("connection_sides")), QStringLiteral("bottom | side | top"));
			QCOMPARE(row.value(QStringLiteral("counterpart_folios")), QStringLiteral("1"));
			QCOMPARE(row.value(QStringLiteral("bridge_uuid")), QStringLiteral("cccccccc-cccc-4ccc-8ccc-cccccccccccc"));
			QCOMPARE(row.value(QStringLiteral("bridge_color")), QStringLiteral("#ff8800"));
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("OK"));
			QCOMPARE(row.value(QStringLiteral("warnings")), QString());
		}

		QCOMPARE(first.value(QStringLiteral("physical_terminal_key")), QStringLiteral("bbbbbbbb-bbbb-4bbb-8bbb-bbbbbbbbbbbb:1"));
		QCOMPARE(first.value(QStringLiteral("physical_index")), QStringLiteral("1"));
		QCOMPARE(first.value(QStringLiteral("terminal_manufacturer")), QStringLiteral("WAGO"));
		QCOMPARE(first.value(QStringLiteral("terminal_designation")), QStringLiteral("2000-1201"));
		QCOMPARE(first.value(QStringLiteral("terminal_description")), QStringLiteral("1mm2 2L terminal block"));
		QCOMPARE(first.value(QStringLiteral("terminal_led")), QStringLiteral("true"));
		QCOMPARE(first.value(QStringLiteral("conductor")), QStringLiteral("XT1-W1"));
		QCOMPARE(first.value(QStringLiteral("connection_count")), QStringLiteral("2"));
		QCOMPARE(first.value(QStringLiteral("connected_terminals")), QStringLiteral("bottom | top"));
		QCOMPARE(first.value(QStringLiteral("connected_cables")), QStringLiteral("CABLE-B | CABLE-A"));
		QCOMPARE(first.value(QStringLiteral("connected_wire_colors")), QStringLiteral("BU | BK"));
		QCOMPARE(first.value(QStringLiteral("connected_wire_sections")), QStringLiteral("2.5 | 1.5"));
		QCOMPARE(first.value(QStringLiteral("connected_conductor_functions")), QStringLiteral("RET | CTRL"));
		QCOMPARE(first.value(QStringLiteral("connected_conductor_uuids")), QStringLiteral("dddddddd-dddd-4ddd-8ddd-dddddddddd03 | dddddddd-dddd-4ddd-8ddd-dddddddddd01"));
		QCOMPARE(first.value(QStringLiteral("connected_conductors")), QStringLiteral("XT1-W2 | XT1-W1"));
		QCOMPARE(first.value(QStringLiteral("counterpart_elements")), QStringLiteral("v2_busstub_n | v2_no_contact"));
		QCOMPARE(first.value(QStringLiteral("counterpart_element_uuids")), QStringLiteral("dc1e8bdf-ce98-5f8e-8e72-8cbe654d0062 | 15e1997c-4eb6-50df-8629-22efc1524f2f"));
		QCOMPARE(first.value(QStringLiteral("counterpart_terminals")), QStringLiteral("tap | bottom"));
		QCOMPARE(first.value(QStringLiteral("connection_side_conductor_uuids")), QStringLiteral("dddddddd-dddd-4ddd-8ddd-dddddddddd03 |  | dddddddd-dddd-4ddd-8ddd-dddddddddd01"));
		QCOMPARE(first.value(QStringLiteral("connection_side_conductors")), QStringLiteral("XT1-W2 |  | XT1-W1"));
		QCOMPARE(first.value(QStringLiteral("connection_side_counterpart_elements")), QStringLiteral("v2_busstub_n |  | v2_no_contact"));
		QCOMPARE(first.value(QStringLiteral("connection_side_counterpart_element_uuids")), QStringLiteral("dc1e8bdf-ce98-5f8e-8e72-8cbe654d0062 |  | 15e1997c-4eb6-50df-8629-22efc1524f2f"));
		QCOMPARE(first.value(QStringLiteral("connection_side_counterpart_terminals")), QStringLiteral("tap |  | bottom"));
		QCOMPARE(first.value(QStringLiteral("connection_side_counterpart_folios")), QStringLiteral("1 |  | 1"));
		QCOMPARE(second.value(QStringLiteral("physical_terminal_key")), QStringLiteral("bbbbbbbb-bbbb-4bbb-8bbb-bbbbbbbbbbbb:2"));
		QCOMPARE(second.value(QStringLiteral("physical_index")), QStringLiteral("2"));
		QCOMPARE(second.value(QStringLiteral("terminal_manufacturer")), QString());
		QCOMPARE(second.value(QStringLiteral("terminal_designation")), QString());
		QCOMPARE(second.value(QStringLiteral("terminal_description")), QString());
		QCOMPARE(second.value(QStringLiteral("terminal_led")), QStringLiteral("false"));
		QCOMPARE(second.value(QStringLiteral("conductor")), QStringLiteral("XT2-W1"));
		QCOMPARE(second.value(QStringLiteral("connection_count")), QStringLiteral("1"));
		QCOMPARE(second.value(QStringLiteral("connected_terminals")), QStringLiteral("top"));
		QCOMPARE(second.value(QStringLiteral("connected_conductor_uuids")), QStringLiteral("dddddddd-dddd-4ddd-8ddd-dddddddddd02"));
		QCOMPARE(second.value(QStringLiteral("connected_conductors")), QStringLiteral("XT2-W1"));
		QCOMPARE(second.value(QStringLiteral("connected_cables")), QStringLiteral("CABLE-A"));
		QCOMPARE(second.value(QStringLiteral("connected_wire_colors")), QStringLiteral("BK"));
		QCOMPARE(second.value(QStringLiteral("connected_wire_sections")), QStringLiteral("1.5"));
		QCOMPARE(second.value(QStringLiteral("connected_conductor_functions")), QStringLiteral("CTRL"));
		QCOMPARE(second.value(QStringLiteral("counterpart_elements")), QStringLiteral("v2_busstub_n"));
		QCOMPARE(second.value(QStringLiteral("counterpart_element_uuids")), QStringLiteral("dc1e8bdf-ce98-5f8e-8e72-8cbe654d0062"));
		QCOMPARE(second.value(QStringLiteral("counterpart_terminals")), QStringLiteral("tap"));

		for (const QHash<QString, QString> &row : {multilevel_first, multilevel_second}) {
			QCOMPARE(row.value(QStringLiteral("physical_terminal_key")), QStringLiteral("bbbbbbbb-bbbb-4bbb-8bbb-bbbbbbbbbbbb:3"));
			QCOMPARE(row.value(QStringLiteral("physical_index")), QStringLiteral("3"));
			QCOMPARE(row.value(QStringLiteral("level_count")), QStringLiteral("2"));
			QCOMPARE(row.value(QStringLiteral("terminal_is_multilevel")), QStringLiteral("true"));
			QCOMPARE(row.value(QStringLiteral("terminal_manufacturer")), QStringLiteral("Phoenix Contact"));
			QCOMPARE(row.value(QStringLiteral("terminal_designation")), QStringLiteral("PT 2,5-2L"));
			QCOMPARE(row.value(QStringLiteral("terminal_led")), QStringLiteral("false"));
			QCOMPARE(row.value(QStringLiteral("terminal_connection_point_count")), QStringLiteral("3"));
			QCOMPARE(row.value(QStringLiteral("terminal_connection_points")), QStringLiteral("bottom | side | top"));
			QCOMPARE(row.value(QStringLiteral("connection_side_count")), QStringLiteral("3"));
			QCOMPARE(row.value(QStringLiteral("connection_sides")), QStringLiteral("bottom | side | top"));
			QCOMPARE(row.value(QStringLiteral("connection_count")), QStringLiteral("0"));
			QCOMPARE(row.value(QStringLiteral("connected_terminals")), QString());
			QCOMPARE(row.value(QStringLiteral("bridge_uuid")), QString());
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("OK"));
			QCOMPARE(row.value(QStringLiteral("warnings")), QString());
		}
		QCOMPARE(multilevel_first.value(QStringLiteral("level")), QStringLiteral("1"));
		QCOMPARE(multilevel_first.value(QStringLiteral("terminal_description")), QStringLiteral("Synthetic double-level terminal level A"));
		QCOMPARE(multilevel_second.value(QStringLiteral("level")), QStringLiteral("2"));
		QCOMPARE(multilevel_second.value(QStringLiteral("terminal_description")), QStringLiteral("Synthetic double-level terminal level B"));
	}
};

QTEST_APPLESS_MAIN(tst_mam_terminal_strip_export)

#include "tst_mam_terminal_strip_export.moc"

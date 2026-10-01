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

QHash<QString, QString> rowByKindAndTargetLabel(
	const QList<QStringList> &rows,
	const QString &kind,
	const QString &target_label)
{
	QHash<QString, QString> values;
	if (rows.isEmpty())
		return values;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("kind"))) != kind
			|| fields.value(header.indexOf(QStringLiteral("target_label"))) != target_label) {
			continue;
		}

		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		return values;
	}
	return values;
}

QHash<QString, QString> rowByKindAndTargetUuid(
	const QList<QStringList> &rows,
	const QString &kind,
	const QString &target_uuid)
{
	QHash<QString, QString> values;
	if (rows.isEmpty())
		return values;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("kind"))) != kind
			|| fields.value(header.indexOf(QStringLiteral("target_uuid"))) != target_uuid) {
			continue;
		}

		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		return values;
	}
	return values;
}

QHash<QString, QString> rowByKindAndDisplayText(
	const QList<QStringList> &rows,
	const QString &kind,
	const QString &display_text)
{
	QHash<QString, QString> values;
	if (rows.isEmpty())
		return values;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("kind"))) != kind
			|| fields.value(header.indexOf(QStringLiteral("display_text"))) != display_text) {
			continue;
		}

		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		return values;
	}
	return values;
}

int rowCountForKind(const QList<QStringList> &rows, const QString &kind)
{
	if (rows.isEmpty())
		return 0;

	const QStringList header = rows.first();
	int count = 0;
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("kind"))) == kind) {
			++count;
		}
	}
	return count;
}

QString readTextFile(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return {};
	return QString::fromUtf8(file.readAll());
}

QString writeTextFile(QTemporaryDir &dir, const QString &name, const QString &content)
{
	const QString path = dir.filePath(name);
	QFile file(path);
	if (!dir.isValid() || !file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		return {};
	file.write(content.toUtf8());
	file.close();
	return path;
}

QString plcMasterDataXml()
{
	return QStringLiteral(
		"<plcMasterData rowHeight=\"8.00\">"
		"<breakPositions/>"
		"<columnWidths/>"
		"<columnVisibility/>"
		"<showHeaders>1</showHeaders>"
		"<plcIOs>"
		"<plcIO type=\"entree_digitale\" address=\"%I0.0\" functionText=\"Start\" comment=\"Panel button\" terminalCount=\"2\">"
		"<terminal>13</terminal><terminal>14</terminal>"
		"</plcIO>"
		"<plcIO type=\"sortie_digitale\" address=\"%Q0.0\" functionText=\"Run\" comment=\"Relay\" terminalCount=\"2\">"
		"<terminal>21</terminal><terminal>22</terminal>"
		"</plcIO>"
		"</plcIOs>"
		"</plcMasterData>");
}

QString plcSlaveInfoXml(
	const QString &label,
	const QString &type,
	const QString &address,
	const QString &function,
	const QString &comment,
	const QString &terminal_1,
	const QString &terminal_2)
{
	return QStringLiteral(
		"<elementInformations>"
		"<elementInformation name=\"label\" show=\"1\">%1</elementInformation>"
		"<elementInformation name=\"plc_type\" show=\"1\">%2</elementInformation>"
		"<elementInformation name=\"plc_address\" show=\"1\">%3</elementInformation>"
		"<elementInformation name=\"plc_function\" show=\"1\">%4</elementInformation>"
		"<elementInformation name=\"plc_comment\" show=\"1\">%5</elementInformation>"
		"<elementInformation name=\"plc_tc\" show=\"1\">2</elementInformation>"
		"<elementInformation name=\"plc_t1\" show=\"1\">%6</elementInformation>"
		"<elementInformation name=\"plc_t2\" show=\"1\">%7</elementInformation>"
		"<elementInformation name=\"plc_t3\" show=\"1\"></elementInformation>"
		"<elementInformation name=\"plc_t4\" show=\"1\"></elementInformation>"
		"</elementInformations>")
			.arg(label, type, address, function, comment, terminal_1, terminal_2);
}

QString plcProjectXml()
{
	QString xml = readTextFile(QFINDTESTDATA("fixtures/master_slave_links_group_index_minimal.qet"));
	if (xml.isEmpty())
		return {};

	xml.replace(
		QStringLiteral("<kindInformation name=\"type\" show=\"1\">coil</kindInformation>"),
		QStringLiteral("<kindInformation name=\"type\" show=\"1\">plc</kindInformation>")
			+ plcMasterDataXml());
	xml.replace(
		QStringLiteral(
			"<elementInformations>\n"
			"                    <elementInformation name=\"label\" show=\"1\">KMS-NO</elementInformation>\n"
			"                </elementInformations>"),
		plcSlaveInfoXml(
			QStringLiteral("KMS-NO"),
			QStringLiteral("Entrée digitale"),
			QStringLiteral("%I0.0"),
			QStringLiteral("Start"),
			QStringLiteral("Panel button"),
			QStringLiteral("13"),
			QStringLiteral("14")));
	xml.replace(
		QStringLiteral(
			"<elementInformations>\n"
			"                    <elementInformation name=\"label\" show=\"1\">KMS-NC</elementInformation>\n"
			"                </elementInformations>"),
		plcSlaveInfoXml(
			QStringLiteral("KMS-NC"),
			QStringLiteral("Sortie digitale"),
			QStringLiteral("%Q0.0"),
			QStringLiteral("Run"),
			QStringLiteral("Relay"),
			QStringLiteral("21"),
			QStringLiteral("22")));
	return xml;
}

QString reportElementXml(
	const QString &type,
	const QString &uuid,
	const QString &linked_uuid,
	const QString &label,
	int x,
	int y)
{
	return QStringLiteral(
		"<element type=\"embed://import/mam_test/%1.elmt\" uuid=\"{%2}\" prefix=\"\" x=\"%3\" y=\"%4\" z=\"10\" orientation=\"0\" freezeLabel=\"false\">"
		"<terminals>"
		"<terminal id=\"0\" x=\"0\" y=\"0\" orientation=\"0\" name=\"\" number=\"\" nameHidden=\"0\"/>"
		"</terminals>"
		"<inputs/>"
		"<links_uuids><link_uuid uuid=\"{%5}\"/></links_uuids>"
		"<elementInformations>"
		"<elementInformation name=\"label\" show=\"1\">%6</elementInformation>"
		"</elementInformations>"
		"<dynamic_texts/>"
		"<texts_groups/>"
		"</element>")
			.arg(type,
				 uuid,
				 QString::number(x),
				 QString::number(y),
				 linked_uuid,
				 label);
}

QString reportDefinitionXml(const QString &type, const QString &name)
{
	return QStringLiteral(
		"<element name=\"%1.elmt\">"
		"<definition type=\"element\" link_type=\"%1\" version=\"0.100.1\" width=\"40\" height=\"40\" hotspot_x=\"20\" hotspot_y=\"20\">"
		"<uuid uuid=\"{%2}\"/>"
		"<names><name lang=\"en\">%3 report</name></names>"
		"<kindInformations/>"
		"<informations>Test-only report fixture element.</informations>"
		"<description>"
		"<input x=\"0\" y=\"0\" tagg=\"label\" size=\"9\" text=\"_\"/>"
		"<line x1=\"-15\" y1=\"0\" x2=\"15\" y2=\"0\" end1=\"none\" end2=\"none\" style=\"line-style:normal;line-weight:normal;filling:none;color:black\" length1=\"1.5\" length2=\"1.5\" antialias=\"false\"/>"
		"<terminal x=\"0\" y=\"0\" orientation=\"n\" name=\"\"/>"
		"</description>"
		"</definition>"
		"</element>")
			.arg(type,
				 type == QStringLiteral("next_report")
					? QStringLiteral("aaaaaaa4-aaaa-4aaa-8aaa-aaaaaaaaaaa4")
					: QStringLiteral("aaaaaaa5-aaaa-4aaa-8aaa-aaaaaaaaaaa5"),
				 name);
}

QString reportProjectXml()
{
	const QString next_uuid = QStringLiteral("44444444-4444-4444-8444-444444444444");
	const QString previous_uuid = QStringLiteral("55555555-5555-4555-8555-555555555555");

	return QStringLiteral(
		"<project title=\"Report Link Fixture\" version=\"0.100.1\">"
		"<properties/>"
		"<usage enabled=\"true\" time_spent=\"0\"/>"
		"<newdiagrams>"
		"<border cols=\"4\" colsize=\"80\" displaycols=\"true\" rows=\"4\" rowsize=\"80\" displayrows=\"true\"/>"
		"<inset title=\"\" author=\"\" date=\"null\" filename=\"\" folio=\"%id/%total\" plant=\"\" locmach=\"\" indexrev=\"\" version=\"\" auto_page_num=\"\" displayAt=\"bottom\"/>"
		"<conductors type=\"multi\" num=\"_\" displaytext=\"1\" onetextperfolio=\"0\"/>"
		"<report label=\"%f-%l%c\"/>"
		"<xrefs/>"
		"<conductors_autonums freeze_new_conductors=\"false\" auto_break_conductors=\"false\" current_autonum=\"\"/>"
		"<folio_autonums/>"
		"<element_autonums freeze_new_elements=\"false\" current_autonum=\"\"/>"
		"<guides/>"
		"</newdiagrams>"
		"<diagram title=\"Report Source\" order=\"1\" version=\"0.100.1-dev\" cols=\"4\" rows=\"4\" colsize=\"80\" rowsize=\"80\" displaycols=\"true\" displayrows=\"true\" displayAt=\"bottom\" folio=\"%id/%total\" author=\"\" date=\"null\" filename=\"\" plant=\"\" locmach=\"\" indexrev=\"\" freezeNewElement=\"false\" freezeNewConductor=\"false\">"
		"<defaultconductor type=\"multi\" num=\"_\" displaytext=\"1\" onetextperfolio=\"0\"/>"
		"<elements>%1</elements>"
		"<conductors/>"
		"<inputs/>"
		"<shapes/>"
		"<images/>"
		"<conductors_autonums/>"
		"</diagram>"
		"<diagram title=\"Report Target\" order=\"2\" version=\"0.100.1-dev\" cols=\"4\" rows=\"4\" colsize=\"80\" rowsize=\"80\" displaycols=\"true\" displayrows=\"true\" displayAt=\"bottom\" folio=\"%id/%total\" author=\"\" date=\"null\" filename=\"\" plant=\"\" locmach=\"\" indexrev=\"\" freezeNewElement=\"false\" freezeNewConductor=\"false\">"
		"<defaultconductor type=\"multi\" num=\"_\" displaytext=\"1\" onetextperfolio=\"0\"/>"
		"<elements>%2</elements>"
		"<conductors/>"
		"<inputs/>"
		"<shapes/>"
		"<images/>"
		"<conductors_autonums/>"
		"</diagram>"
		"<collection>"
		"<category name=\"import\">"
		"<names><name lang=\"en\">Import</name></names>"
		"<category name=\"mam_test\">"
		"<names><name lang=\"en\">MAM Test</name></names>"
		"%3%4"
		"</category>"
		"</category>"
		"</collection>"
		"</project>")
			.arg(reportElementXml(
					 QStringLiteral("next_report"),
					 next_uuid,
					 previous_uuid,
					 QStringLiteral("R-NEXT"),
					 120,
					 120),
				 reportElementXml(
					 QStringLiteral("previous_report"),
					 previous_uuid,
					 next_uuid,
					 QStringLiteral("R-PREV"),
					 220,
					 180),
				 reportDefinitionXml(QStringLiteral("next_report"), QStringLiteral("Next")),
				 reportDefinitionXml(QStringLiteral("previous_report"), QStringLiteral("Previous")));
}

} // namespace

class tst_mam_cross_reference_export : public QObject
{
	Q_OBJECT

private slots:
	void exportsContactCrossReferences()
	{
		const QString fixture = QFINDTESTDATA("fixtures/master_slave_links_group_index_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "master/slave projection fixture project not found");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString export_path = dir.filePath(QStringLiteral("mam_cross_reference.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-cross-reference"),
			fixture,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 2 MAM cross-reference row(s), 0 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 3);
		QCOMPARE(
			rows.first(),
			QStringList({
				QStringLiteral("kind"),
				QStringLiteral("source_service"),
				QStringLiteral("source_role"),
				QStringLiteral("source_label"),
				QStringLiteral("source_uuid"),
				QStringLiteral("source_folio"),
				QStringLiteral("source_position"),
				QStringLiteral("target_role"),
				QStringLiteral("target_label"),
				QStringLiteral("target_uuid"),
				QStringLiteral("target_folio"),
				QStringLiteral("target_position"),
				QStringLiteral("relationship"),
				QStringLiteral("cardinality"),
				QStringLiteral("display_text"),
				QStringLiteral("reference_text"),
				QStringLiteral("status"),
				QStringLiteral("warnings")
			}));

		const QHash<QString, QString> no_row = rowByKindAndTargetLabel(
			rows,
			QStringLiteral("contact_crossref"),
			QStringLiteral("KMS-NO"));
		QVERIFY(!no_row.isEmpty());
		QCOMPARE(no_row.value(QStringLiteral("source_service")), QStringLiteral("ContactCrossRefProjectionService"));
		QCOMPARE(no_row.value(QStringLiteral("source_role")), QStringLiteral("master"));
		QCOMPARE(no_row.value(QStringLiteral("source_label")), QStringLiteral("KMS"));
		QCOMPARE(no_row.value(QStringLiteral("source_uuid")), QStringLiteral("11111111-1111-4111-8111-111111111111"));
		QCOMPARE(no_row.value(QStringLiteral("source_folio")), QStringLiteral("0"));
		QCOMPARE(no_row.value(QStringLiteral("target_role")), QStringLiteral("slave_contact"));
		QCOMPARE(no_row.value(QStringLiteral("target_uuid")), QStringLiteral("22222222-2222-4222-8222-222222222222"));
		QCOMPARE(no_row.value(QStringLiteral("target_folio")), QStringLiteral("0"));
		QCOMPARE(no_row.value(QStringLiteral("relationship")), QStringLiteral("group_index 0"));
		QCOMPARE(no_row.value(QStringLiteral("cardinality")), QStringLiteral("1:n"));
		QCOMPARE(no_row.value(QStringLiteral("display_text")), QStringLiteral("/0"));
		QCOMPARE(no_row.value(QStringLiteral("reference_text")), QStringLiteral("group_index 0"));
		QCOMPARE(no_row.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(no_row.value(QStringLiteral("warnings")), QString());

		const QHash<QString, QString> nc_row = rowByKindAndTargetLabel(
			rows,
			QStringLiteral("contact_crossref"),
			QStringLiteral("KMS-NC"));
		QVERIFY(!nc_row.isEmpty());
		QCOMPARE(nc_row.value(QStringLiteral("relationship")), QStringLiteral("group_index 1"));
		QCOMPARE(nc_row.value(QStringLiteral("cardinality")), QStringLiteral("1:n"));
		QCOMPARE(nc_row.value(QStringLiteral("display_text")), QStringLiteral("/0"));
		QCOMPARE(nc_row.value(QStringLiteral("reference_text")), QStringLiteral("group_index 1"));
		QCOMPARE(nc_row.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(nc_row.value(QStringLiteral("warnings")), QString());
	}

	void exportsPlcIoCrossReferencesWithoutContactDoubleCounting()
	{
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString project_path = writeTextFile(
			dir,
			QStringLiteral("mam_cross_reference_plc.qet"),
			plcProjectXml());
		QVERIFY2(!project_path.isEmpty(), "PLC project fixture write failed");

		const QString export_path = dir.filePath(QStringLiteral("mam_cross_reference_plc.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-cross-reference"),
			project_path,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 2 MAM cross-reference row(s), 0 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 3);

		const QHash<QString, QString> input_row = rowByKindAndTargetLabel(
			rows,
			QStringLiteral("plc_io"),
			QStringLiteral("KMS-NO"));
		QVERIFY(!input_row.isEmpty());
		QCOMPARE(input_row.value(QStringLiteral("source_service")), QStringLiteral("PlcIoProjectionService"));
		QCOMPARE(input_row.value(QStringLiteral("source_role")), QStringLiteral("plc_master_channel"));
		QCOMPARE(input_row.value(QStringLiteral("target_role")), QStringLiteral("plc_slave"));
		QCOMPARE(input_row.value(QStringLiteral("relationship")), QStringLiteral("io_index 0"));
		QCOMPARE(input_row.value(QStringLiteral("cardinality")), QStringLiteral("1:1"));
		QCOMPARE(input_row.value(QStringLiteral("display_text")), QStringLiteral("/0"));
		QCOMPARE(input_row.value(QStringLiteral("reference_text")), QStringLiteral("io_index 0 | %I0.0 | 13,14"));
		QCOMPARE(input_row.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(input_row.value(QStringLiteral("warnings")), QString());

		const QHash<QString, QString> output_row = rowByKindAndTargetLabel(
			rows,
			QStringLiteral("plc_io"),
			QStringLiteral("KMS-NC"));
		QVERIFY(!output_row.isEmpty());
		QCOMPARE(output_row.value(QStringLiteral("relationship")), QStringLiteral("io_index 1"));
		QCOMPARE(output_row.value(QStringLiteral("cardinality")), QStringLiteral("1:1"));
		QCOMPARE(output_row.value(QStringLiteral("reference_text")), QStringLiteral("io_index 1 | %Q0.0 | 21,22"));
		QCOMPARE(output_row.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(output_row.value(QStringLiteral("warnings")), QString());
	}

	void exportsReportFolioBackReferences()
	{
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString project_path = writeTextFile(
			dir,
			QStringLiteral("mam_cross_reference_report.qet"),
			reportProjectXml());
		QVERIFY2(!project_path.isEmpty(), "report project fixture write failed");

		const QString export_path = dir.filePath(QStringLiteral("mam_cross_reference_report.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-cross-reference"),
			project_path,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 2 MAM cross-reference row(s), 0 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 3);

		const QHash<QString, QString> next_row = rowByKindAndTargetUuid(
			rows,
			QStringLiteral("report_folio_backreference"),
			QStringLiteral("55555555-5555-4555-8555-555555555555"));
		QVERIFY(!next_row.isEmpty());
		QCOMPARE(next_row.value(QStringLiteral("source_service")), QStringLiteral("ReportElement"));
		QCOMPARE(next_row.value(QStringLiteral("source_role")), QStringLiteral("next_report"));
		QCOMPARE(next_row.value(QStringLiteral("source_label")), QStringLiteral("R-NEXT"));
		QCOMPARE(next_row.value(QStringLiteral("source_uuid")), QStringLiteral("44444444-4444-4444-8444-444444444444"));
		QCOMPARE(next_row.value(QStringLiteral("source_folio")), QStringLiteral("0"));
		QVERIFY(!next_row.value(QStringLiteral("source_position")).isEmpty());
		QCOMPARE(next_row.value(QStringLiteral("target_role")), QStringLiteral("previous_report"));
		QCOMPARE(next_row.value(QStringLiteral("target_label")), QStringLiteral("R-PREV"));
		QCOMPARE(next_row.value(QStringLiteral("target_folio")), QStringLiteral("1"));
		QVERIFY(!next_row.value(QStringLiteral("target_position")).isEmpty());
		QCOMPARE(next_row.value(QStringLiteral("relationship")), QStringLiteral("next_report_to_previous_report"));
		QCOMPARE(next_row.value(QStringLiteral("cardinality")), QStringLiteral("1:1"));
		QVERIFY(!next_row.value(QStringLiteral("display_text")).isEmpty());
		QVERIFY(next_row.value(QStringLiteral("display_text")) != next_row.value(QStringLiteral("relationship")));
		QCOMPARE(next_row.value(QStringLiteral("reference_text")),
				 QStringLiteral("/1.%1").arg(next_row.value(QStringLiteral("target_position"))));
		QCOMPARE(next_row.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(next_row.value(QStringLiteral("warnings")), QString());

		const QHash<QString, QString> previous_row = rowByKindAndTargetUuid(
			rows,
			QStringLiteral("report_folio_backreference"),
			QStringLiteral("44444444-4444-4444-8444-444444444444"));
		QVERIFY(!previous_row.isEmpty());
		QCOMPARE(previous_row.value(QStringLiteral("source_role")), QStringLiteral("previous_report"));
		QCOMPARE(previous_row.value(QStringLiteral("target_role")), QStringLiteral("next_report"));
		QCOMPARE(previous_row.value(QStringLiteral("relationship")), QStringLiteral("previous_report_to_next_report"));
		QCOMPARE(previous_row.value(QStringLiteral("target_folio")), QStringLiteral("0"));
		QCOMPARE(previous_row.value(QStringLiteral("reference_text")),
				 QStringLiteral("/0.%1").arg(previous_row.value(QStringLiteral("target_position"))));
		QCOMPARE(previous_row.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(previous_row.value(QStringLiteral("warnings")), QString());
	}

	void exportsTerminalStripBackreferences()
	{
		const QString fixture = QFINDTESTDATA("fixtures/terminal_strip_synthetic_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "synthetic terminal-strip fixture project not found");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString export_path = dir.filePath(QStringLiteral("mam_cross_reference_terminal.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-cross-reference"),
			fixture,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 14 MAM cross-reference row(s), 0 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 15);
		QCOMPARE(rowCountForKind(rows, QStringLiteral("terminal_backreference")), 4);
		QCOMPARE(rowCountForKind(rows, QStringLiteral("cable_plan_reference")), 10);

		const QHash<QString, QString> first = rowByKindAndTargetUuid(
			rows,
			QStringLiteral("terminal_backreference"),
			QStringLiteral("aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa1"));
		QVERIFY(!first.isEmpty());
		QCOMPARE(first.value(QStringLiteral("source_service")), QStringLiteral("TerminalStrip"));
		QCOMPARE(first.value(QStringLiteral("source_role")), QStringLiteral("terminal_strip"));
		QCOMPARE(first.value(QStringLiteral("source_label")), QStringLiteral("XT-SYN"));
		QCOMPARE(first.value(QStringLiteral("source_uuid")), QStringLiteral("bbbbbbbb-bbbb-4bbb-8bbb-bbbbbbbbbbbb"));
		QCOMPARE(first.value(QStringLiteral("source_folio")), QString());
		QCOMPARE(first.value(QStringLiteral("source_position")), QStringLiteral("physical 1 | level 1"));
		QCOMPARE(first.value(QStringLiteral("target_role")), QStringLiteral("schematic_terminal"));
		QCOMPARE(first.value(QStringLiteral("target_folio")), QStringLiteral("1"));
		QCOMPARE(first.value(QStringLiteral("target_position")), QStringLiteral("D1"));
		QCOMPARE(first.value(QStringLiteral("relationship")),
				 QStringLiteral("physical_index 1 | level 1 | level_count 1 | bridge cccccccc-cccc-4ccc-8ccc-cccccccccccc"));
		QCOMPARE(first.value(QStringLiteral("cardinality")), QStringLiteral("1:n"));
		QVERIFY2(!first.value(QStringLiteral("display_text")).isEmpty(), "terminal display text should be resolved");
		QVERIFY(first.value(QStringLiteral("reference_text")).contains(QStringLiteral("XT-SYN")));
		QVERIFY(first.value(QStringLiteral("reference_text")).contains(QStringLiteral("physical 1")));
		QVERIFY(first.value(QStringLiteral("reference_text")).contains(first.value(QStringLiteral("display_text"))));
		QCOMPARE(first.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(first.value(QStringLiteral("warnings")), QString());

		const QHash<QString, QString> multilevel_second = rowByKindAndTargetUuid(
			rows,
			QStringLiteral("terminal_backreference"),
			QStringLiteral("aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa4"));
		QVERIFY(!multilevel_second.isEmpty());
		QCOMPARE(multilevel_second.value(QStringLiteral("relationship")),
				 QStringLiteral("physical_index 3 | level 2 | level_count 2"));
		QCOMPARE(multilevel_second.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(multilevel_second.value(QStringLiteral("warnings")), QString());
	}

	void exportsCableCoresAsCablePlanReferences()
	{
		const QString fixture = QFINDTESTDATA("fixtures/terminal_strip_synthetic_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "synthetic terminal-strip fixture project not found");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString export_path = dir.filePath(QStringLiteral("mam_cross_reference_cable.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-cross-reference"),
			fixture,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 14 MAM cross-reference row(s), 0 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rowCountForKind(rows, QStringLiteral("cable_plan_reference")), 10);

		const QHash<QString, QString> cable_b = rowByKindAndDisplayText(
			rows,
			QStringLiteral("cable_plan_reference"),
			QStringLiteral("cable_plan:CABLE-B:XT1-W2"));
		QVERIFY(!cable_b.isEmpty());
		QCOMPARE(cable_b.value(QStringLiteral("source_service")), QStringLiteral("CableCoreCrossRefProjectionService"));
		QCOMPARE(cable_b.value(QStringLiteral("source_role")), QStringLiteral("cable_core"));
		QCOMPARE(cable_b.value(QStringLiteral("source_uuid")), QStringLiteral("dddddddd-dddd-4ddd-8ddd-dddddddddd03"));
		QCOMPARE(cable_b.value(QStringLiteral("source_label")), QStringLiteral("CABLE-B:XT1-W2"));
		QCOMPARE(cable_b.value(QStringLiteral("target_role")), QStringLiteral("cable_plan_row"));
		QCOMPARE(cable_b.value(QStringLiteral("target_uuid")), QStringLiteral("dddddddd-dddd-4ddd-8ddd-dddddddddd03"));
		QCOMPARE(cable_b.value(QStringLiteral("target_label")), QStringLiteral("CABLE-B"));
		QCOMPARE(cable_b.value(QStringLiteral("target_folio")), QString());
		QVERIFY(cable_b.value(QStringLiteral("relationship")).startsWith(QStringLiteral("cable_plan_row | conductor ")));
		QCOMPARE(cable_b.value(QStringLiteral("cardinality")), QStringLiteral("1:1"));
		QVERIFY(cable_b.value(QStringLiteral("reference_text")).contains(QStringLiteral("cable CABLE-B")));
		QVERIFY(cable_b.value(QStringLiteral("reference_text")).contains(QStringLiteral("wire XT1-W2")));
		QVERIFY(cable_b.value(QStringLiteral("reference_text")).contains(QStringLiteral("source ")));
		QVERIFY(cable_b.value(QStringLiteral("reference_text")).contains(QStringLiteral("target ")));
		QCOMPARE(cable_b.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(cable_b.value(QStringLiteral("warnings")), QString());
	}
};

QTEST_MAIN(tst_mam_cross_reference_export)
#include "tst_mam_cross_reference_export.moc"

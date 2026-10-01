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

#include "cablecorecrossrefprojectionservice.h"
#include "crossreferenceprojectionservice.h"
#include "properties/elementdata.h"
#include "qetmessagebox.h"
#include "qetproject.h"

#include <QApplication>
#include <QDomImplementation>
#include <QFile>
#include <QTemporaryDir>

namespace {

const QUuid kMasterUuid(QStringLiteral("{11111111-1111-4111-8111-111111111111}"));
const QUuid kNoSlaveUuid(QStringLiteral("{22222222-2222-4222-8222-222222222222}"));
const QUuid kNcSlaveUuid(QStringLiteral("{33333333-3333-4333-8333-333333333333}"));
const QUuid kNextReportUuid(QStringLiteral("{44444444-4444-4444-8444-444444444444}"));
const QUuid kPreviousReportUuid(QStringLiteral("{55555555-5555-4555-8555-555555555555}"));
const QUuid kTerminalStripUuid(QStringLiteral("{bbbbbbbb-bbbb-4bbb-8bbb-bbbbbbbbbbbb}"));
const QUuid kFirstTerminalUuid(QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa1}"));
const QUuid kSecondTerminalUuid(QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa2}"));
const QUuid kFirstMultilevelTerminalUuid(QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa3}"));
const QUuid kSecondMultilevelTerminalUuid(QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa4}"));
const QUuid kCableBConductorUuid(QStringLiteral("{dddddddd-dddd-4ddd-8ddd-dddddddddd03}"));

QString fixturePath()
{
	return QFINDTESTDATA("fixtures/master_slave_links_group_index_minimal.qet");
}

QString terminalStripFixturePath()
{
	return QFINDTESTDATA("fixtures/terminal_strip_synthetic_minimal.qet");
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
	QString xml = readTextFile(fixturePath());
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
			ElementData::translatedPlcIOType(ElementData::EntreeDigitale),
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
			ElementData::translatedPlcIOType(ElementData::SortieDigitale),
			QStringLiteral("%Q0.0"),
			QStringLiteral("Run"),
			QStringLiteral("Relay"),
			QStringLiteral("21"),
			QStringLiteral("22")));
	return xml;
}

QString reportElementXml(
	const QString &type,
	const QUuid &uuid,
	const QUuid &linked_uuid,
	const QString &label,
	int x,
	int y)
{
	return QStringLiteral(
		"<element type=\"embed://import/mam_test/%1.elmt\" uuid=\"%2\" prefix=\"\" x=\"%3\" y=\"%4\" z=\"10\" orientation=\"0\" freezeLabel=\"false\">"
		"<terminals>"
		"<terminal id=\"0\" x=\"0\" y=\"0\" orientation=\"0\" name=\"\" number=\"\" nameHidden=\"0\"/>"
		"</terminals>"
		"<inputs/>"
		"<links_uuids><link_uuid uuid=\"%5\"/></links_uuids>"
		"<elementInformations>"
		"<elementInformation name=\"label\" show=\"1\">%6</elementInformation>"
		"</elementInformations>"
		"<dynamic_texts/>"
		"<texts_groups/>"
		"</element>")
			.arg(type,
				 uuid.toString(),
				 QString::number(x),
				 QString::number(y),
				 linked_uuid.toString(),
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
					 kNextReportUuid,
					 kPreviousReportUuid,
					 QStringLiteral("R-NEXT"),
					 120,
					 120),
				 reportElementXml(
					 QStringLiteral("previous_report"),
					 kPreviousReportUuid,
					 kNextReportUuid,
					 QStringLiteral("R-PREV"),
					 220,
					 180),
				 reportDefinitionXml(QStringLiteral("next_report"), QStringLiteral("Next")),
				 reportDefinitionXml(QStringLiteral("previous_report"), QStringLiteral("Previous")));
}

const CrossReferenceProjection *referenceForTarget(
	const QList<CrossReferenceProjection> &references,
	const QString &kind,
	const QUuid &target_uuid)
{
	for (const CrossReferenceProjection &reference : references) {
		if (reference.kind == kind && reference.target_uuid == target_uuid)
			return &reference;
	}
	return nullptr;
}

int referenceCountForKind(const QList<CrossReferenceProjection> &references, const QString &kind)
{
	int count = 0;
	for (const CrossReferenceProjection &reference : references) {
		if (reference.kind == kind) {
			++count;
		}
	}
	return count;
}

const CableCoreCrossRefProjection *cableReferenceForWire(
	const QList<CableCoreCrossRefProjection> &references,
	const QString &wire_number)
{
	for (const CableCoreCrossRefProjection &reference : references) {
		if (reference.wire_number == wire_number) {
			return &reference;
		}
	}
	return nullptr;
}

void initHeadlessProjectLoad()
{
	QETProject::setBackupEnabled(false);
	QET::QetMessageBox::setNonInteractive(true);
	QDomImplementation::setInvalidDataPolicy(QDomImplementation::ReturnNullNode);
}

} // namespace

class tst_crossreferenceprojectionservice : public QObject
{
	Q_OBJECT

private slots:
	void projectsContactAssignmentsIntoReadOnlyBackbone()
	{
		initHeadlessProjectLoad();

		const QString fixture = fixturePath();
		QVERIFY2(!fixture.isEmpty(), "master/slave projection fixture project not found");

		QETProject project(fixture);
		QCOMPARE(project.state(), QETProject::Ok);

		CrossReferenceProjectionService service;
		const QList<CrossReferenceProjection> references = service.references(project);
		QCOMPARE(references.size(), 2);

		const CrossReferenceProjection *no_reference =
			referenceForTarget(references, QStringLiteral("contact_crossref"), kNoSlaveUuid);
		QVERIFY(no_reference);
		QCOMPARE(no_reference->source_service, QStringLiteral("ContactCrossRefProjectionService"));
		QCOMPARE(no_reference->source_role, QStringLiteral("master"));
		QCOMPARE(no_reference->source_uuid, kMasterUuid);
		QCOMPARE(no_reference->source_label, QStringLiteral("KMS"));
		QCOMPARE(no_reference->source_folio, 0);
		QCOMPARE(no_reference->target_role, QStringLiteral("slave_contact"));
		QCOMPARE(no_reference->target_label, QStringLiteral("KMS-NO"));
		QCOMPARE(no_reference->target_folio, 0);
		QCOMPARE(no_reference->relationship, QStringLiteral("group_index 0"));
		QCOMPARE(no_reference->cardinality, QStringLiteral("1:n"));
		QCOMPARE(no_reference->display_text, QStringLiteral("/0"));
		QCOMPARE(no_reference->reference_text, QStringLiteral("group_index 0"));
		QCOMPARE(no_reference->status, QStringLiteral("OK"));
		QVERIFY(no_reference->warnings.isEmpty());

		const CrossReferenceProjection *nc_reference =
			referenceForTarget(references, QStringLiteral("contact_crossref"), kNcSlaveUuid);
		QVERIFY(nc_reference);
		QCOMPARE(nc_reference->source_uuid, kMasterUuid);
		QCOMPARE(nc_reference->target_label, QStringLiteral("KMS-NC"));
		QCOMPARE(nc_reference->relationship, QStringLiteral("group_index 1"));
		QCOMPARE(nc_reference->cardinality, QStringLiteral("1:n"));
		QCOMPARE(nc_reference->display_text, QStringLiteral("/0"));
		QCOMPARE(nc_reference->reference_text, QStringLiteral("group_index 1"));
		QCOMPARE(nc_reference->status, QStringLiteral("OK"));
		QVERIFY(nc_reference->warnings.isEmpty());
	}

	void projectsPlcIoIntoReadOnlyBackbone()
	{
		initHeadlessProjectLoad();

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString path = writeTextFile(
			dir,
			QStringLiteral("cross_reference_plc_io.qet"),
			plcProjectXml());
		QVERIFY2(!path.isEmpty(), "PLC project fixture write failed");

		QETProject project(path);
		QCOMPARE(project.state(), QETProject::Ok);

		CrossReferenceProjectionService service;
		const QList<CrossReferenceProjection> references = service.references(project);
		QCOMPARE(references.size(), 2);

		const CrossReferenceProjection *input_reference =
			referenceForTarget(references, QStringLiteral("plc_io"), kNoSlaveUuid);
		QVERIFY(input_reference);
		QCOMPARE(input_reference->source_service, QStringLiteral("PlcIoProjectionService"));
		QCOMPARE(input_reference->source_role, QStringLiteral("plc_master_channel"));
		QCOMPARE(input_reference->source_uuid, kMasterUuid);
		QCOMPARE(input_reference->source_label, QStringLiteral("KMS"));
		QCOMPARE(input_reference->target_role, QStringLiteral("plc_slave"));
		QCOMPARE(input_reference->target_label, QStringLiteral("KMS-NO"));
		QCOMPARE(input_reference->relationship, QStringLiteral("io_index 0"));
		QCOMPARE(input_reference->cardinality, QStringLiteral("1:1"));
		QCOMPARE(input_reference->display_text, QStringLiteral("/0"));
		QCOMPARE(input_reference->reference_text, QStringLiteral("io_index 0 | %I0.0 | 13,14"));
		QCOMPARE(input_reference->status, QStringLiteral("OK"));
		QVERIFY(input_reference->warnings.isEmpty());

		const CrossReferenceProjection *output_reference =
			referenceForTarget(references, QStringLiteral("plc_io"), kNcSlaveUuid);
		QVERIFY(output_reference);
		QCOMPARE(output_reference->relationship, QStringLiteral("io_index 1"));
		QCOMPARE(output_reference->reference_text, QStringLiteral("io_index 1 | %Q0.0 | 21,22"));
		QCOMPARE(output_reference->status, QStringLiteral("OK"));
		QVERIFY(output_reference->warnings.isEmpty());
	}

	void projectsReportFolioBackReferencesWhenLinkTargetIsUnique()
	{
		initHeadlessProjectLoad();

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString path = writeTextFile(
			dir,
			QStringLiteral("cross_reference_report_links.qet"),
			reportProjectXml());
		QVERIFY2(!path.isEmpty(), "report project fixture write failed");

		QETProject project(path);
		QCOMPARE(project.state(), QETProject::Ok);

		CrossReferenceProjectionService service;
		const QList<CrossReferenceProjection> references = service.references(project);
		QCOMPARE(references.size(), 2);

		const CrossReferenceProjection *next_reference =
			referenceForTarget(
				references,
				QStringLiteral("report_folio_backreference"),
				kPreviousReportUuid);
		QVERIFY(next_reference);
		QCOMPARE(next_reference->source_service, QStringLiteral("ReportElement"));
		QCOMPARE(next_reference->source_role, QStringLiteral("next_report"));
		QCOMPARE(next_reference->source_uuid, kNextReportUuid);
		QCOMPARE(next_reference->source_label, QStringLiteral("R-NEXT"));
		QCOMPARE(next_reference->source_folio, 0);
		QVERIFY(!next_reference->source_position.isEmpty());
		QCOMPARE(next_reference->target_role, QStringLiteral("previous_report"));
		QCOMPARE(next_reference->target_label, QStringLiteral("R-PREV"));
		QCOMPARE(next_reference->target_folio, 1);
		QVERIFY(!next_reference->target_position.isEmpty());
		QCOMPARE(next_reference->relationship, QStringLiteral("next_report_to_previous_report"));
		QCOMPARE(next_reference->cardinality, QStringLiteral("1:1"));
		QVERIFY(!next_reference->display_text.isEmpty());
		QVERIFY(next_reference->display_text != next_reference->relationship);
		QCOMPARE(next_reference->reference_text, QStringLiteral("/1.%1").arg(next_reference->target_position));
		QCOMPARE(next_reference->status, QStringLiteral("OK"));
		QVERIFY(next_reference->warnings.isEmpty());

		const CrossReferenceProjection *previous_reference =
			referenceForTarget(
				references,
				QStringLiteral("report_folio_backreference"),
				kNextReportUuid);
		QVERIFY(previous_reference);
		QCOMPARE(previous_reference->source_role, QStringLiteral("previous_report"));
		QCOMPARE(previous_reference->target_role, QStringLiteral("next_report"));
		QCOMPARE(previous_reference->relationship, QStringLiteral("previous_report_to_next_report"));
		QCOMPARE(previous_reference->target_folio, 0);
		QCOMPARE(previous_reference->reference_text, QStringLiteral("/0.%1").arg(previous_reference->target_position));
		QCOMPARE(previous_reference->status, QStringLiteral("OK"));
		QVERIFY(previous_reference->warnings.isEmpty());
	}

	void projectsTerminalStripBackreferencesIntoReadOnlyBackbone()
	{
		initHeadlessProjectLoad();

		const QString fixture = terminalStripFixturePath();
		QVERIFY2(!fixture.isEmpty(), "synthetic terminal-strip fixture project not found");

		QETProject project(fixture);
		QCOMPARE(project.state(), QETProject::Ok);

		CrossReferenceProjectionService service;
		const QList<CrossReferenceProjection> references = service.references(project);
		QCOMPARE(referenceCountForKind(references, QStringLiteral("terminal_backreference")), 4);

		const CrossReferenceProjection *first =
			referenceForTarget(references, QStringLiteral("terminal_backreference"), kFirstTerminalUuid);
		QVERIFY(first);
		QCOMPARE(first->source_service, QStringLiteral("TerminalStrip"));
		QCOMPARE(first->source_role, QStringLiteral("terminal_strip"));
		QCOMPARE(first->source_uuid, kTerminalStripUuid);
		QCOMPARE(first->source_label, QStringLiteral("XT-SYN"));
		QCOMPARE(first->source_folio, -1);
		QCOMPARE(first->source_position, QStringLiteral("physical 1 | level 1"));
		QCOMPARE(first->target_role, QStringLiteral("schematic_terminal"));
		QCOMPARE(first->target_uuid, kFirstTerminalUuid);
		QCOMPARE(first->target_folio, 1);
		QCOMPARE(first->target_position, QStringLiteral("D1"));
		QCOMPARE(first->relationship,
				 QStringLiteral("physical_index 1 | level 1 | level_count 1 | bridge cccccccc-cccc-4ccc-8ccc-cccccccccccc"));
		QCOMPARE(first->cardinality, QStringLiteral("1:n"));
		QVERIFY2(!first->display_text.isEmpty(), "terminal backreference display text should be resolved");
		QVERIFY(first->reference_text.contains(QStringLiteral("XT-SYN")));
		QVERIFY(first->reference_text.contains(QStringLiteral("physical 1")));
		QVERIFY(first->reference_text.contains(first->display_text));
		QCOMPARE(first->status, QStringLiteral("OK"));
		QVERIFY(first->warnings.isEmpty());

		const CrossReferenceProjection *second =
			referenceForTarget(references, QStringLiteral("terminal_backreference"), kSecondTerminalUuid);
		QVERIFY(second);
		QCOMPARE(second->relationship,
				 QStringLiteral("physical_index 2 | level 1 | level_count 1 | bridge cccccccc-cccc-4ccc-8ccc-cccccccccccc"));
		QCOMPARE(second->status, QStringLiteral("OK"));
		QVERIFY(second->warnings.isEmpty());

		const CrossReferenceProjection *multilevel_first =
			referenceForTarget(references, QStringLiteral("terminal_backreference"), kFirstMultilevelTerminalUuid);
		QVERIFY(multilevel_first);
		QCOMPARE(multilevel_first->relationship,
				 QStringLiteral("physical_index 3 | level 1 | level_count 2"));
		QCOMPARE(multilevel_first->status, QStringLiteral("OK"));
		QVERIFY(multilevel_first->warnings.isEmpty());

		const CrossReferenceProjection *multilevel_second =
			referenceForTarget(references, QStringLiteral("terminal_backreference"), kSecondMultilevelTerminalUuid);
		QVERIFY(multilevel_second);
		QCOMPARE(multilevel_second->relationship,
				 QStringLiteral("physical_index 3 | level 2 | level_count 2"));
		QCOMPARE(multilevel_second->status, QStringLiteral("OK"));
		QVERIFY(multilevel_second->warnings.isEmpty());
	}

	void projectsCableCoresAsCablePlanReferencesOnly()
	{
		initHeadlessProjectLoad();

		const QString fixture = terminalStripFixturePath();
		QVERIFY2(!fixture.isEmpty(), "synthetic terminal-strip fixture project not found");

		QETProject project(fixture);
		QCOMPARE(project.state(), QETProject::Ok);

		CableCoreCrossRefProjectionService cable_service;
		const QList<CableCoreCrossRefProjection> cable_references = cable_service.references(project);
		QCOMPARE(cable_references.size(), 10);

		const CableCoreCrossRefProjection *cable_b =
			cableReferenceForWire(cable_references, QStringLiteral("XT1-W2"));
		QVERIFY(cable_b);
		QCOMPARE(cable_b->conductor_uuid, kCableBConductorUuid);
		QCOMPARE(cable_b->cable, QStringLiteral("CABLE-B"));
		QCOMPARE(cable_b->wire_color, QStringLiteral("BU"));
		QCOMPARE(cable_b->wire_section, QStringLiteral("2.5"));
		QCOMPARE(cable_b->function, QStringLiteral("RET"));
		QCOMPARE(cable_b->display_text, QStringLiteral("cable_plan:CABLE-B:XT1-W2"));
		QVERIFY(cable_b->relationship.startsWith(QStringLiteral("cable_plan_row | conductor ")));
		QVERIFY(cable_b->reference_text.contains(QStringLiteral("cable CABLE-B")));
		QVERIFY(cable_b->reference_text.contains(QStringLiteral("wire XT1-W2")));
		QVERIFY(cable_b->reference_text.contains(QStringLiteral("source ")));
		QVERIFY(cable_b->reference_text.contains(QStringLiteral("target ")));
		QVERIFY(cable_b->warnings.isEmpty());

		CrossReferenceProjectionService service;
		const QList<CrossReferenceProjection> references = service.references(project);
		QCOMPARE(referenceCountForKind(references, QStringLiteral("cable_plan_reference")), 10);

		const CrossReferenceProjection *backbone_reference =
			referenceForTarget(references, QStringLiteral("cable_plan_reference"), kCableBConductorUuid);
		QVERIFY(backbone_reference);
		QCOMPARE(backbone_reference->source_service, QStringLiteral("CableCoreCrossRefProjectionService"));
		QCOMPARE(backbone_reference->source_role, QStringLiteral("cable_core"));
		QCOMPARE(backbone_reference->source_uuid, kCableBConductorUuid);
		QCOMPARE(backbone_reference->source_label, QStringLiteral("CABLE-B:XT1-W2"));
		QCOMPARE(backbone_reference->target_role, QStringLiteral("cable_plan_row"));
		QCOMPARE(backbone_reference->target_uuid, kCableBConductorUuid);
		QCOMPARE(backbone_reference->target_label, QStringLiteral("CABLE-B"));
		QCOMPARE(backbone_reference->target_folio, -1);
		QCOMPARE(backbone_reference->cardinality, QStringLiteral("1:1"));
		QCOMPARE(backbone_reference->display_text, QStringLiteral("cable_plan:CABLE-B:XT1-W2"));
		QVERIFY(backbone_reference->reference_text.contains(QStringLiteral("source ")));
		QVERIFY(backbone_reference->reference_text.contains(QStringLiteral("target ")));
		QCOMPARE(backbone_reference->status, QStringLiteral("OK"));
		QVERIFY(backbone_reference->warnings.isEmpty());
	}
};

int main(int argc, char **argv)
{
	qputenv("QT_HASH_SEED", "0");
	QCoreApplication::setOrganizationName("QElectroTech");
	QCoreApplication::setOrganizationDomain("qelectrotech.org");
	QCoreApplication::setApplicationName("QElectroTech");
	QApplication app(argc, argv);
	tst_crossreferenceprojectionservice tc;
	return QTest::qExec(&tc, argc, argv);
}

#include "tst_crossreferenceprojectionservice.moc"

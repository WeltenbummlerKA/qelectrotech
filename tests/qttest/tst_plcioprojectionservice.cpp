/*
	Copyright 2006-2026 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech. If not, see <http://www.gnu.org/licenses/>.
*/
#include <QtTest>

#include "plcioprojectionservice.h"
#include "qetinformation.h"
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

QString fixturePath()
{
	return QFINDTESTDATA("fixtures/master_slave_links_group_index_minimal.qet");
}

QString readTextFile(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qWarning().noquote() << QStringLiteral("cannot read %1").arg(path);
		return {};
	}
	return QString::fromUtf8(file.readAll());
}

QString writeFixture(QTemporaryDir &dir, const QString &content)
{
	const QString path = dir.filePath(QStringLiteral("plc_io_projection.qet"));
	QFile file(path);
	if (!dir.isValid() || !file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		qWarning().noquote() << QStringLiteral("cannot write %1").arg(path);
		return {};
	}
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
		"<plcIO type=\"entree_digitale\" address=\"%I0.0\" functionText=\"Start\" comment=\"Panel button\" terminalCount=\"3\">"
		"<terminal>1</terminal><terminal>2</terminal>"
		"</plcIO>"
		"<plcIO type=\"sortie_digitale\" address=\"\" functionText=\"Run\" comment=\"Motor relay\" terminalCount=\"2\">"
		"<terminal>3</terminal><terminal>4</terminal>"
		"</plcIO>"
		"<plcIO type=\"entree_analogique\" address=\"%IW64\" functionText=\"Pressure\" comment=\"Sensor\" terminalCount=\"1\">"
		"<terminal>5</terminal>"
		"</plcIO>"
		"</plcIOs>"
		"</plcMasterData>");
}

QString plcSlaveInfoXml(const QString &label, const QString &address, const QString &function)
{
	return QStringLiteral(
		"<elementInformations>"
		"<elementInformation name=\"label\" show=\"1\">%1</elementInformation>"
		"<elementInformation name=\"plc_type\" show=\"1\">%2</elementInformation>"
		"<elementInformation name=\"plc_address\" show=\"1\">%3</elementInformation>"
		"<elementInformation name=\"plc_function\" show=\"1\">%4</elementInformation>"
		"<elementInformation name=\"plc_comment\" show=\"1\">Panel button</elementInformation>"
		"<elementInformation name=\"plc_tc\" show=\"1\">3</elementInformation>"
		"<elementInformation name=\"plc_t1\" show=\"1\">1</elementInformation>"
		"<elementInformation name=\"plc_t2\" show=\"1\">2</elementInformation>"
		"<elementInformation name=\"plc_t3\" show=\"1\"></elementInformation>"
		"<elementInformation name=\"plc_t4\" show=\"1\"></elementInformation>"
		"</elementInformations>")
			.arg(
				label,
				ElementData::translatedPlcIOType(ElementData::EntreeDigitale),
				address,
				function);
}

QString plcProjectXml(const QString &link_replacement)
{
	QString xml = readTextFile(fixturePath());
	if (xml.isEmpty()) {
		return {};
	}

	xml.replace(
		QStringLiteral("<kindInformation name=\"type\" show=\"1\">coil</kindInformation>"),
		QStringLiteral("<kindInformation name=\"type\" show=\"1\">plc</kindInformation>")
			+ plcMasterDataXml());
	xml.replace(
		QStringLiteral("<link_uuid uuid=\"{33333333-3333-4333-8333-333333333333}\" group_index=\"1\"/>"),
		link_replacement);
	xml.replace(
		QStringLiteral(
			"<elementInformations>\n"
			"                    <elementInformation name=\"label\" show=\"1\">KMS-NO</elementInformation>\n"
			"                </elementInformations>"),
		plcSlaveInfoXml(
			QStringLiteral("KMS-NO"),
			QStringLiteral("%I0.0"),
			QStringLiteral("Start")));
	xml.replace(
		QStringLiteral(
			"<elementInformations>\n"
			"                    <elementInformation name=\"label\" show=\"1\">KMS-NC</elementInformation>\n"
			"                </elementInformations>"),
		plcSlaveInfoXml(
			QStringLiteral("KMS-NC"),
			QStringLiteral("%I0.0"),
			QStringLiteral("Start")));
	return xml;
}

const PlcIoProjection *projectionFor(
	const QList<PlcIoProjection> &projections,
	int io_index,
	const QUuid &slave_uuid = QUuid())
{
	for (const PlcIoProjection &projection : projections) {
		if (projection.io_index == io_index
			&& (slave_uuid.isNull() || projection.linked_slave_uuid == slave_uuid)) {
			return &projection;
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

class tst_plcioprojectionservice : public QObject
{
	Q_OBJECT

private slots:
	void projectsPlcIoChannelsAndValidationFlags()
	{
		initHeadlessProjectLoad();

		const QString xml = plcProjectXml(QStringLiteral(
			"<link_uuid uuid=\"{33333333-3333-4333-8333-333333333333}\" group_index=\"0\"/>"));
		QVERIFY2(!xml.isEmpty(), "PLC fixture XML not readable");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString path = writeFixture(dir, xml);
		QVERIFY2(!path.isEmpty(), "PLC fixture write failed");

		QETProject project(path);
		QCOMPARE(project.state(), QETProject::Ok);

		PlcIoProjectionService service;
		const QList<PlcIoProjection> projections = service.channels(project);
		QCOMPARE(projections.size(), 4);

		const PlcIoProjection *start = projectionFor(projections, 0, kNoSlaveUuid);
		QVERIFY(start);
		QCOMPARE(start->master_uuid, kMasterUuid);
		QCOMPARE(start->master_label, QStringLiteral("KMS"));
		QCOMPARE(start->folio, 0);
		QCOMPARE(start->type, ElementData::EntreeDigitale);
		QCOMPARE(start->direction, PlcIoProjection::Input);
		QCOMPARE(start->address, QStringLiteral("%I0.0"));
		QCOMPARE(start->function, QStringLiteral("Start"));
		QCOMPARE(start->comment, QStringLiteral("Panel button"));
		QCOMPARE(start->terminal_count, 3);
		QCOMPARE(start->terminal_labels, QStringList({QStringLiteral("1"), QStringLiteral("2")}));
		QCOMPARE(start->linked_slave_label, QStringLiteral("KMS-NO"));
		QCOMPARE(start->linked_slave_folio, 0);
		QCOMPARE(start->linked_slave_terminal_count, 2);
		QVERIFY(!start->unlinked);
		QVERIFY(start->duplicate_group_index);
		QVERIFY(!start->out_of_range_group_index);
		QVERIFY(!start->empty_address);
		QVERIFY(start->terminal_label_count_mismatch);
		QVERIFY(start->insufficient_slave_terminal_count);
		QVERIFY(!start->stale_plc_copy);
		QVERIFY(start->stale_plc_copy_fields.isEmpty());
		QCOMPARE(
			start->warnings,
			QStringList({
				QStringLiteral("terminal label count 2 does not match terminal_count 3"),
				QStringLiteral("slave terminal count 2 is less than terminal_count 3"),
				QStringLiteral("duplicate group_index 0 assignment")
			}));

		const PlcIoProjection *duplicate = projectionFor(projections, 0, kNcSlaveUuid);
		QVERIFY(duplicate);
		QCOMPARE(duplicate->linked_slave_label, QStringLiteral("KMS-NC"));
		QVERIFY(duplicate->duplicate_group_index);
		QVERIFY(duplicate->terminal_label_count_mismatch);
		QVERIFY(duplicate->insufficient_slave_terminal_count);
		QVERIFY(!duplicate->stale_plc_copy);

		const PlcIoProjection *run = projectionFor(projections, 1);
		QVERIFY(run);
		QCOMPARE(run->type, ElementData::SortieDigitale);
		QCOMPARE(run->direction, PlcIoProjection::Output);
		QCOMPARE(run->address, QString());
		QCOMPARE(run->function, QStringLiteral("Run"));
		QCOMPARE(run->terminal_count, 2);
		QVERIFY(run->unlinked);
		QVERIFY(!run->duplicate_group_index);
		QVERIFY(!run->out_of_range_group_index);
		QVERIFY(run->empty_address);
		QVERIFY(!run->terminal_label_count_mismatch);
		QVERIFY(!run->insufficient_slave_terminal_count);
		QCOMPARE(run->warnings, QStringList({QStringLiteral("empty address"), QStringLiteral("unlinked")}));

		const PlcIoProjection *pressure = projectionFor(projections, 2);
		QVERIFY(pressure);
		QCOMPARE(pressure->type, ElementData::EntreeAnalogique);
		QCOMPARE(pressure->direction, PlcIoProjection::Input);
		QCOMPARE(pressure->terminal_labels, QStringList({QStringLiteral("5")}));
		QVERIFY(pressure->unlinked);
		QCOMPARE(pressure->warnings, QStringList({QStringLiteral("unlinked")}));
	}

	void outOfRangeGroupIndexIsProjected()
	{
		initHeadlessProjectLoad();

		const QString xml = plcProjectXml(QStringLiteral(
			"<link_uuid uuid=\"{33333333-3333-4333-8333-333333333333}\" group_index=\"99\"/>"));
		QVERIFY2(!xml.isEmpty(), "PLC fixture XML not readable");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString path = writeFixture(dir, xml);
		QVERIFY2(!path.isEmpty(), "PLC fixture write failed");

		QETProject project(path);
		QCOMPARE(project.state(), QETProject::Ok);

		PlcIoProjectionService service;
		const QList<PlcIoProjection> projections = service.channels(project);

		const PlcIoProjection *bad_link = projectionFor(projections, 99, kNcSlaveUuid);
		QVERIFY(bad_link);
		QCOMPARE(bad_link->master_uuid, kMasterUuid);
		QCOMPARE(bad_link->master_label, QStringLiteral("KMS"));
		QCOMPARE(bad_link->linked_slave_label, QStringLiteral("KMS-NC"));
		QVERIFY(bad_link->out_of_range_group_index);
		QVERIFY(!bad_link->unlinked);
		QVERIFY(!bad_link->duplicate_group_index);
		QCOMPARE(bad_link->linked_slave_terminal_count, 2);
		QCOMPARE(bad_link->warnings, QStringList({QStringLiteral("group_index 99 out of range")}));
	}

	void staleSlavePlcCopyIsWarningOnly()
	{
		initHeadlessProjectLoad();

		QString xml = plcProjectXml(QStringLiteral(
			"<link_uuid uuid=\"{33333333-3333-4333-8333-333333333333}\" group_index=\"1\"/>"));
		xml.replace(
			QStringLiteral("<elementInformation name=\"plc_address\" show=\"1\">%I0.0</elementInformation>"),
			QStringLiteral("<elementInformation name=\"plc_address\" show=\"1\">%I9.9</elementInformation>"));
		QVERIFY2(!xml.isEmpty(), "PLC fixture XML not readable");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString path = writeFixture(dir, xml);
		QVERIFY2(!path.isEmpty(), "PLC fixture write failed");

		QETProject project(path);
		QCOMPARE(project.state(), QETProject::Ok);

		PlcIoProjectionService service;
		const QList<PlcIoProjection> projections = service.channels(project);

		const PlcIoProjection *start = projectionFor(projections, 0, kNoSlaveUuid);
		QVERIFY(start);
		QVERIFY(start->stale_plc_copy);
		QCOMPARE(start->stale_plc_copy_fields, QStringList({QETInformation::ELMT_PLC_ADDRESS}));
		QCOMPARE(
			start->warnings,
			QStringList({
				QStringLiteral("terminal label count 2 does not match terminal_count 3"),
				QStringLiteral("slave terminal count 2 is less than terminal_count 3"),
				QStringLiteral("stale plc copy fields: plc_address")
			}));
		QCOMPARE(start->address, QStringLiteral("%I0.0"));
	}
};

int main(int argc, char **argv)
{
	qputenv("QT_HASH_SEED", "0");
	qputenv("QT_QPA_PLATFORM", "offscreen");
	QCoreApplication::setOrganizationName("QElectroTech");
	QCoreApplication::setOrganizationDomain("qelectrotech.org");
	QCoreApplication::setApplicationName("QElectroTech");
	QApplication app(argc, argv);
	tst_plcioprojectionservice tc;
	return QTest::qExec(&tc, argc, argv);
}

#include "tst_plcioprojectionservice.moc"

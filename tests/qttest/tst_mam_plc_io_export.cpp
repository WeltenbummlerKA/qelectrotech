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

#include "cli_test_utils.h"

#include <QFile>
#include <QTemporaryDir>

namespace {

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
		"<elementInformation name=\"plc_type\" show=\"1\">Entrée digitale</elementInformation>"
		"<elementInformation name=\"plc_address\" show=\"1\">%2</elementInformation>"
		"<elementInformation name=\"plc_function\" show=\"1\">%3</elementInformation>"
		"<elementInformation name=\"plc_comment\" show=\"1\">Panel button</elementInformation>"
		"<elementInformation name=\"plc_tc\" show=\"1\">3</elementInformation>"
		"<elementInformation name=\"plc_t1\" show=\"1\">1</elementInformation>"
		"<elementInformation name=\"plc_t2\" show=\"1\">2</elementInformation>"
		"<elementInformation name=\"plc_t3\" show=\"1\"></elementInformation>"
		"<elementInformation name=\"plc_t4\" show=\"1\"></elementInformation>"
		"</elementInformations>")
			.arg(
				label,
				address,
				function);
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
		QStringLiteral("<link_uuid uuid=\"{33333333-3333-4333-8333-333333333333}\" group_index=\"1\"/>"),
		QStringLiteral("<link_uuid uuid=\"{33333333-3333-4333-8333-333333333333}\" group_index=\"0\"/>"));
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
			QStringLiteral("%I0.1"),
			QStringLiteral("Wrong Start")));
	return xml;
}

QHash<QString, QString> rowByIoIndexAndSlave(const QList<QStringList> &rows, int io_index, const QString &slave_label)
{
	QHash<QString, QString> values;
	const QStringList header = rows.value(0);
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("io_index"))) != QString::number(io_index)
			|| fields.value(header.indexOf(QStringLiteral("linked_slave_label"))) != slave_label) {
			continue;
		}
		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		return values;
	}
	return values;
}

} // namespace

class tst_mam_plc_io_export : public QObject
{
	Q_OBJECT

private slots:
	void exportsProjectionBackedMamPlcIoCsv()
	{
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString project_path = writeTextFile(
			dir,
			QStringLiteral("mam_plc_io_export.qet"),
			plcProjectXml());
		QVERIFY2(!project_path.isEmpty(), "PLC project fixture write failed");

		const QString export_path = dir.filePath(QStringLiteral("mam_plc_io.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-plc-io"),
			project_path,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 4 MAM PLC IO channel(s), 4 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 5);
		QCOMPARE(
			rows.first(),
			QStringList({
				QStringLiteral("master_label"),
				QStringLiteral("master_uuid"),
				QStringLiteral("folio"),
				QStringLiteral("io_index"),
				QStringLiteral("type"),
				QStringLiteral("direction"),
				QStringLiteral("address"),
				QStringLiteral("function"),
				QStringLiteral("comment"),
				QStringLiteral("terminal_count"),
				QStringLiteral("terminal_labels"),
				QStringLiteral("linked_slave_label"),
				QStringLiteral("linked_slave_uuid"),
				QStringLiteral("linked_slave_folio"),
				QStringLiteral("linked_slave_terminal_count"),
				QStringLiteral("status"),
				QStringLiteral("warnings")
			}));

		const QHash<QString, QString> no_row = rowByIoIndexAndSlave(rows, 0, QStringLiteral("KMS-NO"));
		QVERIFY(!no_row.isEmpty());
		QCOMPARE(no_row.value(QStringLiteral("master_label")), QStringLiteral("KMS"));
		QCOMPARE(no_row.value(QStringLiteral("master_uuid")), QStringLiteral("11111111-1111-4111-8111-111111111111"));
		QCOMPARE(no_row.value(QStringLiteral("folio")), QStringLiteral("0"));
		QCOMPARE(no_row.value(QStringLiteral("type")), QStringLiteral("Entrée digitale"));
		QCOMPARE(no_row.value(QStringLiteral("direction")), QStringLiteral("input"));
		QCOMPARE(no_row.value(QStringLiteral("address")), QStringLiteral("%I0.0"));
		QCOMPARE(no_row.value(QStringLiteral("function")), QStringLiteral("Start"));
		QCOMPARE(no_row.value(QStringLiteral("comment")), QStringLiteral("Panel button"));
		QCOMPARE(no_row.value(QStringLiteral("terminal_count")), QStringLiteral("3"));
		QCOMPARE(no_row.value(QStringLiteral("terminal_labels")), QStringLiteral("1,2"));
		QCOMPARE(no_row.value(QStringLiteral("linked_slave_uuid")), QStringLiteral("22222222-2222-4222-8222-222222222222"));
		QCOMPARE(no_row.value(QStringLiteral("linked_slave_folio")), QStringLiteral("0"));
		QCOMPARE(no_row.value(QStringLiteral("linked_slave_terminal_count")), QStringLiteral("2"));
		QCOMPARE(no_row.value(QStringLiteral("status")), QStringLiteral("WARNING"));
		QVERIFY(no_row.value(QStringLiteral("warnings")).contains(QStringLiteral("terminal label count 2 does not match terminal_count 3")));
		QVERIFY(no_row.value(QStringLiteral("warnings")).contains(QStringLiteral("slave terminal count 2 is less than terminal_count 3")));
		QVERIFY(no_row.value(QStringLiteral("warnings")).contains(QStringLiteral("duplicate group_index 0 assignment")));

		const QHash<QString, QString> nc_row = rowByIoIndexAndSlave(rows, 0, QStringLiteral("KMS-NC"));
		QVERIFY(!nc_row.isEmpty());
		QVERIFY(nc_row.value(QStringLiteral("warnings")).contains(QStringLiteral("duplicate group_index 0 assignment")));

		const QHash<QString, QString> output_row = rowByIoIndexAndSlave(rows, 1, QString());
		QVERIFY(!output_row.isEmpty());
		QCOMPARE(output_row.value(QStringLiteral("direction")), QStringLiteral("output"));
		QCOMPARE(output_row.value(QStringLiteral("address")), QString());
		QVERIFY(output_row.value(QStringLiteral("warnings")).contains(QStringLiteral("empty address")));
	}
};

QTEST_MAIN(tst_mam_plc_io_export)
#include "tst_mam_plc_io_export.moc"

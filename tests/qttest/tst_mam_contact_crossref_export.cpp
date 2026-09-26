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

QHash<QString, QString> rowBySlaveLabel(const QList<QStringList> &rows, const QString &slave_label)
{
	QHash<QString, QString> values;
	if (rows.isEmpty())
		return values;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("slave_label"))) != slave_label)
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

class tst_mam_contact_crossref_export : public QObject
{
	Q_OBJECT

private slots:
	void exportsProjectionBackedMamContactCrossRefCsv()
	{
		const QString fixture = QFINDTESTDATA("fixtures/master_slave_links_group_index_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "master/slave projection fixture project not found");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString export_path = dir.filePath(QStringLiteral("mam_contact_crossref.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-contact-crossref"),
			fixture,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 2 MAM Contact/CrossRef assignment(s), 0 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 3);
		QCOMPARE(
			rows.first(),
			QStringList({
				QStringLiteral("master_label"),
				QStringLiteral("master_uuid"),
				QStringLiteral("master_folio"),
				QStringLiteral("slave_label"),
				QStringLiteral("slave_uuid"),
				QStringLiteral("slave_folio"),
				QStringLiteral("group_index"),
				QStringLiteral("group_index_resolves"),
				QStringLiteral("group_type"),
				QStringLiteral("group_subtype"),
				QStringLiteral("group_contact_count"),
				QStringLiteral("group_terminal_count"),
				QStringLiteral("group_terminal_labels"),
				QStringLiteral("slave_contact_type"),
				QStringLiteral("slave_contact_subtype"),
				QStringLiteral("slave_contact_count"),
				QStringLiteral("duplicate_group_assignment"),
				QStringLiteral("status"),
				QStringLiteral("warnings")
			}));

		const QHash<QString, QString> no_row = rowBySlaveLabel(rows, QStringLiteral("KMS-NO"));
		QVERIFY(!no_row.isEmpty());
		QCOMPARE(no_row.value(QStringLiteral("master_label")), QStringLiteral("KMS"));
		QCOMPARE(no_row.value(QStringLiteral("master_uuid")), QStringLiteral("11111111-1111-4111-8111-111111111111"));
		QCOMPARE(no_row.value(QStringLiteral("master_folio")), QStringLiteral("0"));
		QCOMPARE(no_row.value(QStringLiteral("slave_uuid")), QStringLiteral("22222222-2222-4222-8222-222222222222"));
		QCOMPARE(no_row.value(QStringLiteral("slave_folio")), QStringLiteral("0"));
		QCOMPARE(no_row.value(QStringLiteral("group_index")), QStringLiteral("0"));
		QCOMPARE(no_row.value(QStringLiteral("group_index_resolves")), QStringLiteral("true"));
		QCOMPARE(no_row.value(QStringLiteral("group_type")), QStringLiteral("NO"));
		QCOMPARE(no_row.value(QStringLiteral("group_subtype")), QStringLiteral("simple"));
		QCOMPARE(no_row.value(QStringLiteral("group_contact_count")), QStringLiteral("1"));
		QCOMPARE(no_row.value(QStringLiteral("group_terminal_count")), QStringLiteral("2"));
		QCOMPARE(no_row.value(QStringLiteral("group_terminal_labels")), QStringLiteral("13,14"));
		QCOMPARE(no_row.value(QStringLiteral("slave_contact_type")), QStringLiteral("NO"));
		QCOMPARE(no_row.value(QStringLiteral("slave_contact_subtype")), QStringLiteral("simple"));
		QCOMPARE(no_row.value(QStringLiteral("slave_contact_count")), QStringLiteral("1"));
		QCOMPARE(no_row.value(QStringLiteral("duplicate_group_assignment")), QStringLiteral("false"));
		QCOMPARE(no_row.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(no_row.value(QStringLiteral("warnings")), QString());

		const QHash<QString, QString> nc_row = rowBySlaveLabel(rows, QStringLiteral("KMS-NC"));
		QVERIFY(!nc_row.isEmpty());
		QCOMPARE(nc_row.value(QStringLiteral("group_index")), QStringLiteral("1"));
		QCOMPARE(nc_row.value(QStringLiteral("group_type")), QStringLiteral("NC"));
		QCOMPARE(nc_row.value(QStringLiteral("group_terminal_labels")), QStringLiteral("21,22"));
		QCOMPARE(nc_row.value(QStringLiteral("status")), QStringLiteral("OK"));
	}

	void exportsExistingProjectionWarnings()
	{
		const QString fixture = QFINDTESTDATA("fixtures/master_slave_links_group_index_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "master/slave projection fixture project not found");

		QString xml = QString::fromUtf8(CliTestUtils::readFile(fixture));
		QVERIFY2(!xml.isEmpty(), "fixture content not readable");
		const QString nc_link = QStringLiteral(
			"<link_uuid uuid=\"{33333333-3333-4333-8333-333333333333}\" group_index=\"1\"/>");
		QVERIFY2(xml.contains(nc_link), "expected NC link XML not found in fixture");
		xml.replace(nc_link, QStringLiteral(
			"<link_uuid uuid=\"{33333333-3333-4333-8333-333333333333}\" group_index=\"0\"/>"));

		const QString nc_state = QStringLiteral("<kindInformation name=\"state\" show=\"1\">NC</kindInformation>");
		QVERIFY2(xml.contains(nc_state), "expected NC slave state XML not found in fixture");
		xml.replace(nc_state, QStringLiteral("<kindInformation name=\"state\" show=\"1\">NO</kindInformation>"));

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString variant = writeFixtureVariant(
			dir,
			QStringLiteral("duplicate_group_assignment.qet"),
			xml);
		QVERIFY2(!variant.isEmpty(), "duplicate assignment variant write failed");

		const QString export_path = dir.filePath(QStringLiteral("mam_contact_crossref_warnings.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-contact-crossref"),
			variant,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 2 MAM Contact/CrossRef assignment(s), 2 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 3);

		const QHash<QString, QString> no_row = rowBySlaveLabel(rows, QStringLiteral("KMS-NO"));
		QVERIFY(!no_row.isEmpty());
		QCOMPARE(no_row.value(QStringLiteral("group_index")), QStringLiteral("0"));
		QCOMPARE(no_row.value(QStringLiteral("group_index_resolves")), QStringLiteral("true"));
		QCOMPARE(no_row.value(QStringLiteral("duplicate_group_assignment")), QStringLiteral("true"));
		QCOMPARE(no_row.value(QStringLiteral("status")), QStringLiteral("WARNING"));
		QCOMPARE(no_row.value(QStringLiteral("warnings")), QStringLiteral("duplicate group_index 0 assignment"));

		const QHash<QString, QString> duplicate_row = rowBySlaveLabel(rows, QStringLiteral("KMS-NC"));
		QVERIFY(!duplicate_row.isEmpty());
		QCOMPARE(duplicate_row.value(QStringLiteral("group_index")), QStringLiteral("0"));
		QCOMPARE(duplicate_row.value(QStringLiteral("group_index_resolves")), QStringLiteral("true"));
		QCOMPARE(duplicate_row.value(QStringLiteral("group_type")), QStringLiteral("NO"));
		QCOMPARE(duplicate_row.value(QStringLiteral("slave_contact_type")), QStringLiteral("NO"));
		QCOMPARE(duplicate_row.value(QStringLiteral("duplicate_group_assignment")), QStringLiteral("true"));
		QCOMPARE(duplicate_row.value(QStringLiteral("status")), QStringLiteral("WARNING"));
		QCOMPARE(duplicate_row.value(QStringLiteral("warnings")), QStringLiteral("duplicate group_index 0 assignment"));
	}
};

QTEST_MAIN(tst_mam_contact_crossref_export)
#include "tst_mam_contact_crossref_export.moc"

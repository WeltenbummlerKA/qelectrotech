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

QHash<QString, QString> rowByTypeAndLabel(
	const QList<QStringList> &rows,
	const QString &record_type,
	const QString &label)
{
	QHash<QString, QString> values;
	if (rows.isEmpty())
		return values;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("record_type"))) != record_type)
			continue;
		if (fields.value(header.indexOf(QStringLiteral("label"))) != label)
			continue;

		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		return values;
	}
	return values;
}

} // namespace

class tst_mam_summary_export : public QObject
{
	Q_OBJECT

private slots:
	void exportsSummaryRowsInStableOrder()
	{
		const QString fixture = QFINDTESTDATA("fixtures/workflow_exports_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "workflow export fixture project not found");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString export_path = dir.filePath(QStringLiteral("mam_summary_order.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-summary"),
			fixture,
			export_path
		});
		QCOMPARE(result.exit_code, 0);

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 8);

		QStringList ordered_wire_numbers;
		const QStringList header = rows.first();
		for (int row = 1; row < rows.size(); ++row) {
			const QStringList fields = rows.at(row);
			QCOMPARE(fields.value(header.indexOf(QStringLiteral("record_type"))),
				QStringLiteral("terminal_potential"));
			QCOMPARE(fields.value(header.indexOf(QStringLiteral("source_export"))),
				QStringLiteral("mam-terminal-potential"));
			ordered_wire_numbers << fields.value(header.indexOf(QStringLiteral("label")));
		}
		QCOMPARE(ordered_wire_numbers, QStringList({
			QStringLiteral("W005"), QStringLiteral("W006"), QStringLiteral("W007"),
			QStringLiteral("W008"), QStringLiteral("W009"), QStringLiteral("W010"),
			QStringLiteral("W011")
		}));
	}

	void exportsContactRowsAsFirstMamWorkingList()
	{
		const QString fixture = QFINDTESTDATA("fixtures/master_slave_links_group_index_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "master/slave projection fixture project not found");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString export_path = dir.filePath(QStringLiteral("mam_summary_contact.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-summary"),
			fixture,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 2 MAM summary row(s), 0 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 3);
		QCOMPARE(
			rows.first(),
			QStringList({
				QStringLiteral("record_type"),
				QStringLiteral("folio"),
				QStringLiteral("item_uuid"),
				QStringLiteral("label"),
				QStringLiteral("role"),
				QStringLiteral("category"),
				QStringLiteral("address_or_terminal"),
				QStringLiteral("linked_item"),
				QStringLiteral("status"),
				QStringLiteral("warnings"),
				QStringLiteral("source_export")
			}));

		const QHash<QString, QString> no_row = rowByTypeAndLabel(
			rows,
			QStringLiteral("contact_crossref"),
			QStringLiteral("KMS-NO"));
		QVERIFY(!no_row.isEmpty());
		QCOMPARE(no_row.value(QStringLiteral("folio")), QStringLiteral("0"));
		QCOMPARE(no_row.value(QStringLiteral("item_uuid")), QStringLiteral("22222222-2222-4222-8222-222222222222"));
		QCOMPARE(no_row.value(QStringLiteral("role")), QStringLiteral("NO"));
		QCOMPARE(no_row.value(QStringLiteral("category")), QStringLiteral("NO"));
		QCOMPARE(no_row.value(QStringLiteral("address_or_terminal")), QStringLiteral("group_index 0"));
		QCOMPARE(no_row.value(QStringLiteral("linked_item")), QStringLiteral("KMS"));
		QCOMPARE(no_row.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(no_row.value(QStringLiteral("warnings")), QString());
		QCOMPARE(no_row.value(QStringLiteral("source_export")), QStringLiteral("mam-contact-crossref"));
	}

	void exportsExistingSummaryWarnings()
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
			QStringLiteral("empty_summary_wire_number.qet"),
			xml);
		QVERIFY2(!variant.isEmpty(), "empty wire-number variant write failed");

		const QString export_path = dir.filePath(QStringLiteral("mam_summary_warnings.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-summary"),
			variant,
			export_path
		});
		QCOMPARE(result.exit_code, 0);
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 7 MAM summary row(s), 1 warning row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 8);

		const QSet<QString> types = CliTestUtils::columnSet(rows, QStringLiteral("record_type"));
		QCOMPARE(types, QSet<QString>({QStringLiteral("terminal_potential")}));

		bool found_warning = false;
		const QStringList header = rows.first();
		for (int row = 1; row < rows.size(); ++row) {
			const QStringList fields = rows.at(row);
			if (fields.value(header.indexOf(QStringLiteral("status"))) != QStringLiteral("WARNING"))
				continue;
			found_warning = true;
			QCOMPARE(fields.value(header.indexOf(QStringLiteral("record_type"))), QStringLiteral("terminal_potential"));
			QCOMPARE(fields.value(header.indexOf(QStringLiteral("label"))), QString());
			QCOMPARE(fields.value(header.indexOf(QStringLiteral("role"))), QStringLiteral("conductor"));
			QCOMPARE(fields.value(header.indexOf(QStringLiteral("warnings"))), QStringLiteral("empty potential wire number"));
			QCOMPARE(fields.value(header.indexOf(QStringLiteral("source_export"))), QStringLiteral("mam-terminal-potential"));
		}
		QVERIFY2(found_warning, "expected terminal/potential warning row not found");
	}
};

QTEST_APPLESS_MAIN(tst_mam_summary_export)

#include "tst_mam_summary_export.moc"

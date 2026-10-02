#include <QtTest>

#include "cli_test_utils.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QTemporaryDir>

namespace {

QSet<QString> sortedWireSet()
{
	return {
		QStringLiteral("W005"),
		QStringLiteral("W006"),
		QStringLiteral("W007"),
		QStringLiteral("W008"),
		QStringLiteral("W009"),
		QStringLiteral("W010"),
		QStringLiteral("W011"),
	};
}

QHash<QString, QString> rowByPotentialAndDirection(
	const QList<QStringList> &rows,
	const QString &potential,
	const QString &direction)
{
	QHash<QString, QString> values;
	if (rows.isEmpty())
		return values;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("potential"))) != potential
			|| fields.value(header.indexOf(QStringLiteral("direction"))) != direction) {
			continue;
		}

		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		return values;
	}
	return values;
}

QList<QHash<QString, QString>> rowsByPotential(
	const QList<QStringList> &rows,
	const QString &potential)
{
	QList<QHash<QString, QString>> matches;
	if (rows.isEmpty())
		return matches;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("potential"))) != potential)
			continue;

		QHash<QString, QString> values;
		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		matches << values;
	}
	return matches;
}

QHash<QString, QString> rowByPotentialStatusAndDiagnostic(
	const QList<QStringList> &rows,
	const QString &potential,
	const QString &status,
	const QString &diagnostic)
{
	const QList<QHash<QString, QString>> matches = rowsByPotential(rows, potential);
	for (const QHash<QString, QString> &values : matches) {
		if (values.value(QStringLiteral("status")) == status
			&& values.value(QStringLiteral("diagnostics")).contains(diagnostic)) {
			return values;
		}
	}
	return {};
}

QString currentPathContinuationExample()
{
	const QStringList candidates {
		QString::fromLocal8Bit(QET_TEST_SOURCE_DIR) + QStringLiteral("/examples/MAM_Strompfade_2Seiten.qet"),
		QDir::current().absoluteFilePath(QStringLiteral("examples/MAM_Strompfade_2Seiten.qet")),
		QDir::current().absoluteFilePath(QStringLiteral("../../examples/MAM_Strompfade_2Seiten.qet"))
	};
	for (const QString &candidate : candidates) {
		if (QFile::exists(candidate)) {
			return candidate;
		}
	}
	return {};
}

bool continuationCliUsesOffscreenPlatform()
{
#ifdef Q_OS_WIN
	return false;
#else
	return true;
#endif
}

bool replaceOnce(QString &text, const QString &before, const QString &after)
{
	const int index = text.indexOf(before);
	if (index < 0)
		return false;

	text.replace(index, before.size(), after);
	return true;
}

QString writeContinuationDiagnosticsEdgeFixture(const QString &source, QTemporaryDir &out_dir)
{
	QString text = QString::fromUtf8(CliTestUtils::readFile(source));
	if (text.isEmpty())
		return {};

	bool ok = true;
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformation show=\"1\" name=\"potential\">1N</elementInformation>"),
		QStringLiteral("<elementInformation show=\"1\" name=\"potential\"></elementInformation>"));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformation show=\"1\" name=\"potential\">1L2</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.1</elementInformation>"),
		QStringLiteral("<elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.1</elementInformation>"));
	ok &= replaceOnce(
		text,
		QStringLiteral("type=\"embed://import/mam/mam_potential_previous_h.elmt\" z=\"10\" uuid=\"{c05b4616-1228-4bb4-beec-44190f357e97}\""),
		QStringLiteral("type=\"embed://import/mam/mam_potential_next_h.elmt\" z=\"10\" uuid=\"{c05b4616-1228-4bb4-beec-44190f357e97}\""));
	if (!ok)
		return {};

	const QString path = out_dir.filePath(QStringLiteral("mam_continuation_diagnostics_edge.qet"));
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return {};
	file.write(text.toUtf8());
	return path;
}

} // namespace

class tst_cli_export_equivalence : public QObject
{
	Q_OBJECT

private slots:
	void wiringNetsCablesWiresBomAndInfoShareCoreFacts()
	{
		const QString binary = QStringLiteral(QET_TEST_BINARY_PATH);
		QVERIFY2(QFile::exists(binary),
				 qPrintable(QStringLiteral("qelectrotech binary not found at '%1'").arg(binary)));

		const QString fixture = QFINDTESTDATA("fixtures/workflow_exports_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "export equivalence fixture project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());

		const QString nets_path = out_dir.filePath(QStringLiteral("nets.json"));
		const QString wiring_path = out_dir.filePath(QStringLiteral("wiring.csv"));
		const QString cables_path = out_dir.filePath(QStringLiteral("cables.csv"));
		const QString wires_path = out_dir.filePath(QStringLiteral("wires.csv"));
		const QString bom_path = out_dir.filePath(QStringLiteral("bom.csv"));
		const QString info_path = out_dir.filePath(QStringLiteral("info.json"));

		const QList<QPair<QString, QString>> commands {
			{QStringLiteral("--export-nets"), nets_path},
			{QStringLiteral("--export-wiring"), wiring_path},
			{QStringLiteral("--export-cables"), cables_path},
			{QStringLiteral("--export-wires"), wires_path},
			{QStringLiteral("--export-bom"), bom_path},
			{QStringLiteral("--info"), info_path},
		};

		for (const auto &command : commands) {
			const CliTestUtils::CliResult result =
				CliTestUtils::runQetCli({command.first, fixture, command.second});
			QVERIFY2(result.exit_code == 0,
					 qPrintable(QStringLiteral("%1 failed with exit %2\nstdout: %3\nstderr: %4")
									.arg(command.first)
									.arg(result.exit_code)
									.arg(result.stdout_text.left(500))
									.arg(result.stderr_text.left(500))));
			QVERIFY2(QFile::exists(command.second),
					 qPrintable(QStringLiteral("%1 did not create output").arg(command.first)));
			QVERIFY2(QFileInfo(command.second).size() > 0,
					 qPrintable(QStringLiteral("%1 created an empty output").arg(command.first)));
		}

		const QSet<QString> expected_wires = sortedWireSet();

		const QJsonObject info = CliTestUtils::readJsonObject(info_path);
		QCOMPARE(info.value(QStringLiteral("diagrams")).toInt(), 1);
		QCOMPARE(info.value(QStringLiteral("elements")).toInt(), 9);
		QCOMPARE(info.value(QStringLiteral("conductors")).toInt(), expected_wires.size());

		const QJsonArray nets = CliTestUtils::readJsonObject(nets_path).value(QStringLiteral("list")).toArray();
		QCOMPARE(nets.size(), expected_wires.size());
		QSet<QString> net_wires;
		for (const QJsonValue &value : nets) {
			const QJsonObject net = value.toObject();
			net_wires.insert(net.value(QStringLiteral("wire_no")).toString());
			QCOMPARE(net.value(QStringLiteral("terminals")).toArray().size(), 2);
		}
		QCOMPARE(net_wires, expected_wires);

		const QList<QStringList> wiring_rows =
			CliTestUtils::parseSemicolonCsv(QString::fromUtf8(CliTestUtils::readFile(wiring_path)));
		QCOMPARE(wiring_rows.size() - 1, expected_wires.size());
		QCOMPARE(CliTestUtils::columnSet(wiring_rows, QStringLiteral("wire_number")), expected_wires);

		const QList<QStringList> cables_rows =
			CliTestUtils::parseSemicolonCsv(QString::fromUtf8(CliTestUtils::readFile(cables_path)));
		QCOMPARE(cables_rows.size() - 1, expected_wires.size());
		QCOMPARE(CliTestUtils::columnSet(cables_rows, QStringLiteral("Couleur du fil")), QSet<QString>{QStringLiteral("BK")});
		QCOMPARE(CliTestUtils::columnSet(cables_rows, QStringLiteral("Section du fil")), QSet<QString>{QStringLiteral("1.5")});
		QCOMPARE(CliTestUtils::columnSet(cables_rows, QStringLiteral("Fonction")), QSet<QString>{QStringLiteral("CTRL")});

		const QStringList wire_lines =
			QString::fromUtf8(CliTestUtils::readFile(wires_path)).split(QLatin1Char('\n'), Qt::SkipEmptyParts);
		QCOMPARE(QSet<QString>(wire_lines.cbegin(), wire_lines.cend()), expected_wires);

		const QList<QStringList> bom_rows =
			CliTestUtils::parseSemicolonCsv(QString::fromUtf8(CliTestUtils::readFile(bom_path)));
		QCOMPARE(bom_rows.size() - 1, info.value(QStringLiteral("elements")).toInt());
		QCOMPARE(CliTestUtils::columnSet(bom_rows, QStringLiteral("folio")), QSet<QString>{QStringLiteral("1 OF 1")});
	}

	void potentialContinuationDiagnosticsReportWarningsAndErrors()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString export_path = out_dir.filePath(QStringLiteral("mam_continuation.csv"));

		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-continuation"),
			fixture,
			export_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(result.exit_code == 0,
				 qPrintable(QStringLiteral("continuation export failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(result.exit_code)
								.arg(result.stdout_text.left(500))
								.arg(result.stderr_text.left(500))));
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 8 MAM continuation row(s), 5 warning row(s), 2 error row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 9);
		QCOMPARE(
			rows.first(),
			QStringList({
				QStringLiteral("potential"),
				QStringLiteral("signal"),
				QStringLiteral("voltage"),
				QStringLiteral("continuation_uuid"),
				QStringLiteral("folio"),
				QStringLiteral("grid"),
				QStringLiteral("path"),
				QStringLiteral("direction"),
				QStringLiteral("chain"),
				QStringLiteral("chain_order"),
				QStringLiteral("visible_xref"),
				QStringLiteral("target_uuid"),
				QStringLiteral("target_folio"),
				QStringLiteral("target_grid"),
				QStringLiteral("target_path"),
				QStringLiteral("computed_xref"),
				QStringLiteral("cardinality"),
				QStringLiteral("relationship"),
				QStringLiteral("status"),
				QStringLiteral("diagnostics")
			}));

		const QHash<QString, QString> l1_out = rowByPotentialAndDirection(
			rows,
			QStringLiteral("1L1"),
			QStringLiteral("right"));
		QVERIFY(!l1_out.isEmpty());
		QCOMPARE(l1_out.value(QStringLiteral("status")), QStringLiteral("OK"));
		QCOMPARE(l1_out.value(QStringLiteral("cardinality")), QStringLiteral("1:1"));
		QCOMPARE(l1_out.value(QStringLiteral("relationship")), QStringLiteral("auto-point-to-point"));
		QCOMPARE(l1_out.value(QStringLiteral("computed_xref")), QStringLiteral("/2.0"));
		QCOMPARE(l1_out.value(QStringLiteral("visible_xref")), QStringLiteral("/2.0"));

		const QHash<QString, QString> l1_in = rowByPotentialAndDirection(
			rows,
			QStringLiteral("1L1"),
			QStringLiteral("left"));
		QVERIFY(!l1_in.isEmpty());
		QCOMPARE(l1_in.value(QStringLiteral("status")), QStringLiteral("WARNING"));
		QCOMPARE(l1_in.value(QStringLiteral("computed_xref")), QStringLiteral("/1.5"));
		QCOMPARE(l1_in.value(QStringLiteral("visible_xref")), QStringLiteral("/1.0"));
		QVERIFY(l1_in.value(QStringLiteral("diagnostics")).contains(QStringLiteral("stale visible xref")));

		const QHash<QString, QString> n1 = rowByPotentialAndDirection(
			rows,
			QStringLiteral("1N"),
			QStringLiteral("down"));
		QVERIFY(!n1.isEmpty());
		QCOMPARE(n1.value(QStringLiteral("status")), QStringLiteral("ERROR"));
		QCOMPARE(n1.value(QStringLiteral("cardinality")), QStringLiteral("unresolved"));
		QVERIFY(n1.value(QStringLiteral("diagnostics")).contains(QStringLiteral("missing continuation counterpart")));
	}

	void potentialContinuationDiagnosticsCatchEdgeErrors()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString edge_fixture = writeContinuationDiagnosticsEdgeFixture(fixture, out_dir);
		QVERIFY2(!edge_fixture.isEmpty(), "failed to prepare edge-case continuation fixture");

		const QString export_path = out_dir.filePath(QStringLiteral("mam_continuation_edge.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-continuation"),
			edge_fixture,
			export_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(result.exit_code == 0,
				 qPrintable(QStringLiteral("edge continuation export failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(result.exit_code)
								.arg(result.stdout_text.left(500))
								.arg(result.stderr_text.left(500))));
		QVERIFY(result.stdout_text.contains(QStringLiteral("Exported 8 MAM continuation row(s), 3 warning row(s), 5 error row(s)")));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 9);

		const QHash<QString, QString> empty_potential = rowByPotentialStatusAndDiagnostic(
			rows,
			QString(),
			QStringLiteral("ERROR"),
			QStringLiteral("empty potential or signal"));
		QVERIFY(!empty_potential.isEmpty());
		QCOMPARE(empty_potential.value(QStringLiteral("direction")), QStringLiteral("down"));
		QCOMPARE(empty_potential.value(QStringLiteral("cardinality")), QStringLiteral("unresolved"));

		const QList<QHash<QString, QString>> ambiguous_l1 = rowsByPotential(rows, QStringLiteral("1L1"));
		QCOMPARE(ambiguous_l1.size(), 3);
		for (const QHash<QString, QString> &row : ambiguous_l1) {
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("WARNING"));
			QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("ambiguous"));
			QVERIFY(row.value(QStringLiteral("diagnostics")).contains(QStringLiteral("ambiguous continuation group without chain order")));
		}

		const QHash<QString, QString> missing_l2 = rowByPotentialStatusAndDiagnostic(
			rows,
			QStringLiteral("1L2"),
			QStringLiteral("ERROR"),
			QStringLiteral("missing continuation counterpart"));
		QVERIFY(!missing_l2.isEmpty());

		const QList<QHash<QString, QString>> contradictory_l3 = rowsByPotential(rows, QStringLiteral("1L3"));
		QCOMPARE(contradictory_l3.size(), 2);
		for (const QHash<QString, QString> &row : contradictory_l3) {
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("ERROR"));
			QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("1:1"));
			QCOMPARE(row.value(QStringLiteral("direction")), QStringLiteral("right"));
			QVERIFY(row.value(QStringLiteral("diagnostics")).contains(QStringLiteral("contradictory continuation direction")));
		}
	}
};

QTEST_APPLESS_MAIN(tst_cli_export_equivalence)

#include "tst_cli_export_equivalence.moc"

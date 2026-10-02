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

QList<QHash<QString, QString>> rowsBySignal(
	const QList<QStringList> &rows,
	const QString &signal)
{
	QList<QHash<QString, QString>> matches;
	if (rows.isEmpty())
		return matches;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("signal"))) != signal)
			continue;

		QHash<QString, QString> values;
		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		matches << values;
	}
	return matches;
}

QList<QHash<QString, QString>> rowsByChain(
	const QList<QStringList> &rows,
	const QString &chain)
{
	QList<QHash<QString, QString>> matches;
	if (rows.isEmpty())
		return matches;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("chain"))) != chain)
			continue;

		QHash<QString, QString> values;
		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		matches << values;
	}
	return matches;
}

QList<QHash<QString, QString>> rowsByMamPairId(
	const QList<QStringList> &rows,
	const QString &pair_id)
{
	QList<QHash<QString, QString>> matches;
	if (rows.isEmpty())
		return matches;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("mam_pair_id"))) != pair_id)
			continue;

		QHash<QString, QString> values;
		for (int column = 0; column < header.size(); ++column)
			values.insert(header.at(column), fields.value(column));
		matches << values;
	}
	return matches;
}

QList<QHash<QString, QString>> rowsBySuggestedMamPairId(
	const QList<QStringList> &rows,
	const QString &pair_id)
{
	QList<QHash<QString, QString>> matches;
	if (rows.isEmpty())
		return matches;

	const QStringList header = rows.first();
	for (int row = 1; row < rows.size(); ++row) {
		const QStringList fields = rows.at(row);
		if (fields.value(header.indexOf(QStringLiteral("suggested_mam_pair_id"))) != pair_id)
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

void verifyNoMigrationSuggestion(const QHash<QString, QString> &row)
{
	QVERIFY(row.value(QStringLiteral("suggested_mam_pair_id")).isEmpty());
	QVERIFY(row.value(QStringLiteral("migration_recommendation")).isEmpty());
}

bool continuationCliUsesOffscreenPlatform();

QList<QStringList> exportMamContinuationRows(
	const QString &fixture,
	QTemporaryDir &out_dir,
	const QString &name)
{
	const QString export_path = out_dir.filePath(name);
	const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
		QStringLiteral("--export-mam-continuation"),
		fixture,
		export_path
	}, 30000, continuationCliUsesOffscreenPlatform());
	if (result.exit_code != 0) {
		QTest::qFail(qPrintable(QStringLiteral("continuation export failed with exit %1\nstdout: %2\nstderr: %3")
						.arg(result.exit_code)
						.arg(result.stdout_text.left(500))
						.arg(result.stderr_text.left(500))),
					__FILE__,
					__LINE__);
		return {};
	}
	return CliTestUtils::parseSemicolonCsv(
		QString::fromUtf8(CliTestUtils::readFile(export_path)));
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

QString writeContinuationConflictingSignalFixture(const QString &source, QTemporaryDir &out_dir)
{
	QString text = QString::fromUtf8(CliTestUtils::readFile(source));
	if (text.isEmpty())
		return {};

	bool ok = true;
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"),
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"signal\">SIG-CONFLICT</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"),
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">2L1</elementInformation><elementInformation show=\"1\" name=\"signal\">SIG-CONFLICT</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"));
	if (!ok)
		return {};

	const QString path = out_dir.filePath(QStringLiteral("mam_continuation_conflicting_signal.qet"));
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return {};
	file.write(text.toUtf8());
	return path;
}

QString chainInfoXml(const QString &chain, const QString &order)
{
	QString text = QStringLiteral("<elementInformation show=\"1\" name=\"chain\">%1</elementInformation>")
		.arg(chain);
	if (!order.isEmpty()) {
		text += QStringLiteral("<elementInformation show=\"1\" name=\"chain_order\">%1</elementInformation>")
			.arg(order);
	}
	return text;
}

QString continuationInfoXml(
	const QString &potential,
	const QString &xref,
	const QString &chain,
	const QString &order,
	const QString &voltage = QString())
{
	QString text = QStringLiteral("<elementInformations>"
		"<elementInformation show=\"1\" name=\"potential\">%1</elementInformation>"
		"<elementInformation show=\"1\" name=\"xref\">%2</elementInformation>")
			.arg(potential, xref);
	if (!voltage.isEmpty()) {
		text += QStringLiteral("<elementInformation show=\"1\" name=\"voltage\">%1</elementInformation>")
			.arg(voltage);
	}
	text += chainInfoXml(chain, order);
	text += QStringLiteral("</elementInformations>");
	return text;
}

QString mamContinuationInfoXml(
	const QString &potential,
	const QString &xref,
	const QString &mam_continuation_id,
	const QString &mam_pair_id,
	const QString &mam_chain_id,
	const QString &mam_chain_order,
	const QString &legacy_chain = QString(),
	const QString &legacy_chain_order = QString())
{
	QString text = QStringLiteral("<elementInformations>"
		"<elementInformation show=\"1\" name=\"potential\">%1</elementInformation>"
		"<elementInformation show=\"1\" name=\"xref\">%2</elementInformation>")
			.arg(potential, xref);
	if (!mam_continuation_id.isEmpty()) {
		text += QStringLiteral("<elementInformation show=\"1\" name=\"mam_continuation_id\">%1</elementInformation>")
			.arg(mam_continuation_id);
	}
	if (!mam_pair_id.isEmpty()) {
		text += QStringLiteral("<elementInformation show=\"1\" name=\"mam_pair_id\">%1</elementInformation>")
			.arg(mam_pair_id);
	}
	if (!mam_chain_id.isEmpty()) {
		text += QStringLiteral("<elementInformation show=\"1\" name=\"mam_chain_id\">%1</elementInformation>")
			.arg(mam_chain_id);
	}
	if (!mam_chain_order.isEmpty()) {
		text += QStringLiteral("<elementInformation show=\"1\" name=\"mam_chain_order\">%1</elementInformation>")
			.arg(mam_chain_order);
	}
	if (!legacy_chain.isEmpty()) {
		text += chainInfoXml(legacy_chain, legacy_chain_order);
	}
	text += QStringLiteral("</elementInformations>");
	return text;
}

QString writeContinuationChainDiagnosticsFixture(const QString &source, QTemporaryDir &out_dir)
{
	QString text = QString::fromUtf8(CliTestUtils::readFile(source));
	if (text.isEmpty())
		return {};

	bool ok = true;
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"),
		continuationInfoXml(
			QStringLiteral("1L1"),
			QStringLiteral("/1.5"),
			QStringLiteral("CHAIN_OK"),
			QStringLiteral("1"),
			QStringLiteral("400VAC")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L2</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.1</elementInformation></elementInformations>"),
		continuationInfoXml(
			QStringLiteral("1L2"),
			QStringLiteral("/2.0"),
			QStringLiteral("CHAIN_OK"),
			QStringLiteral("2")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L3</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.2</elementInformation></elementInformations>"),
		continuationInfoXml(
			QStringLiteral("1L3"),
			QStringLiteral("/2.0"),
			QStringLiteral("CHAIN_BAD"),
			QStringLiteral("0")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1N</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.3</elementInformation><elementInformation show=\"1\" name=\"voltage\">0VAC</elementInformation></elementInformations>"),
		continuationInfoXml(
			QStringLiteral("1N"),
			QStringLiteral("/1.2"),
			QStringLiteral("CHAIN_BAD"),
			QStringLiteral("2"),
			QStringLiteral("0VAC")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">2N</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.4</elementInformation><elementInformation show=\"1\" name=\"voltage\">0VAC</elementInformation></elementInformations>"),
		continuationInfoXml(
			QStringLiteral("2N"),
			QStringLiteral("/1.1"),
			QStringLiteral("CHAIN_BAD"),
			QStringLiteral("1"),
			QStringLiteral("0VAC")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"),
		continuationInfoXml(
			QStringLiteral("1L1"),
			QStringLiteral("/1.5"),
			QStringLiteral("CHAIN_BAD"),
			QString()));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L2</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.1</elementInformation></elementInformations>"),
		continuationInfoXml(
			QStringLiteral("1L2"),
			QStringLiteral("/1.5"),
			QStringLiteral("CHAIN_BAD"),
			QStringLiteral("0")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L3</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.2</elementInformation></elementInformations>"),
		continuationInfoXml(
			QStringLiteral("1L3"),
			QStringLiteral("/1.5"),
			QStringLiteral("CHAIN_OK"),
			QStringLiteral("3")));
	if (!ok)
		return {};

	const QString path = out_dir.filePath(QStringLiteral("mam_continuation_chain_diagnostics.qet"));
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return {};
	file.write(text.toUtf8());
	return path;
}

QString writeContinuationNamespacedPairDiagnosticsFixture(const QString &source, QTemporaryDir &out_dir)
{
	QString text = QString::fromUtf8(CliTestUtils::readFile(source));
	if (text.isEmpty())
		return {};

	bool ok = true;
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("1L1"),
			QStringLiteral("/2.0"),
			QStringLiteral("CONT-1"),
			QStringLiteral("PAIR_OK"),
			QString(),
			QString()));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L2</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.1</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("PAIR_SINGLE"),
			QStringLiteral("/2.1"),
			QStringLiteral("CONT-2"),
			QStringLiteral("PAIR_SINGLE"),
			QString(),
			QString()));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L3</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.2</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("PAIR_THREE_A"),
			QStringLiteral("/2.2"),
			QStringLiteral("CONT-3"),
			QStringLiteral("PAIR_THREE"),
			QString(),
			QString()));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1N</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.3</elementInformation><elementInformation show=\"1\" name=\"voltage\">0VAC</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("PAIR_CHAIN_A"),
			QStringLiteral("/2.3"),
			QStringLiteral("CONT-4"),
			QStringLiteral("PAIR_CHAIN"),
			QStringLiteral("CHAIN_CONFLICT"),
			QStringLiteral("1")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">2N</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.4</elementInformation><elementInformation show=\"1\" name=\"voltage\">0VAC</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("PAIR_CHAIN_B"),
			QStringLiteral("/2.4"),
			QStringLiteral("CONT-5"),
			QStringLiteral("PAIR_CHAIN"),
			QStringLiteral("CHAIN_CONFLICT"),
			QStringLiteral("2")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("1L1"),
			QStringLiteral("/1.5"),
			QStringLiteral("CONT-6"),
			QStringLiteral("PAIR_OK"),
			QString(),
			QString()));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L2</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.1</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("PAIR_THREE_B"),
			QStringLiteral("/1.1"),
			QStringLiteral("CONT-7"),
			QStringLiteral("PAIR_THREE"),
			QString(),
			QString()));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L3</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.2</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("PAIR_THREE_C"),
			QStringLiteral("/1.2"),
			QStringLiteral("CONT-8"),
			QStringLiteral("PAIR_THREE"),
			QString(),
			QString()));
	if (!ok)
		return {};

	const QString path = out_dir.filePath(QStringLiteral("mam_continuation_namespaced_pair_diagnostics.qet"));
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return {};
	file.write(text.toUtf8());
	return path;
}

QString writeContinuationNamespacedChainDiagnosticsFixture(const QString &source, QTemporaryDir &out_dir)
{
	QString text = QString::fromUtf8(CliTestUtils::readFile(source));
	if (text.isEmpty())
		return {};

	bool ok = true;
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("MAM_CHAIN_A"),
			QStringLiteral("/1.5"),
			QStringLiteral("CONT-1"),
			QString(),
			QStringLiteral("MAM_CHAIN_OK"),
			QStringLiteral("1")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L2</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.1</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("MAM_CHAIN_B"),
			QStringLiteral("/2.0"),
			QStringLiteral("CONT-2"),
			QString(),
			QStringLiteral("MAM_CHAIN_OK"),
			QStringLiteral("2")));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L3</elementInformation><elementInformation show=\"1\" name=\"xref\">/1.2</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("MAM_CHAIN_C"),
			QStringLiteral("/1.5"),
			QStringLiteral("CONT-8"),
			QString(),
			QStringLiteral("MAM_CHAIN_OK"),
			QStringLiteral("3")));
	if (!ok)
		return {};

	const QString path = out_dir.filePath(QStringLiteral("mam_continuation_namespaced_chain_diagnostics.qet"));
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return {};
	file.write(text.toUtf8());
	return path;
}

QString writeContinuationPairCardinalityFixture(const QString &source, QTemporaryDir &out_dir)
{
	QString text = QString::fromUtf8(CliTestUtils::readFile(source));
	if (text.isEmpty())
		return {};

	bool ok = true;
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L1</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.0</elementInformation><elementInformation show=\"1\" name=\"voltage\">400VAC</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("1L1"),
			QStringLiteral("/2.0"),
			QStringLiteral("CARD-1"),
			QStringLiteral("PAIR_TOO_MANY"),
			QString(),
			QString()));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L2</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.1</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("1L2"),
			QStringLiteral("/2.1"),
			QStringLiteral("CARD-2"),
			QStringLiteral("PAIR_TOO_MANY"),
			QString(),
			QString()));
	ok &= replaceOnce(
		text,
		QStringLiteral("<elementInformations><elementInformation show=\"1\" name=\"potential\">1L3</elementInformation><elementInformation show=\"1\" name=\"xref\">/2.2</elementInformation></elementInformations>"),
		mamContinuationInfoXml(
			QStringLiteral("1L3"),
			QStringLiteral("/2.2"),
			QStringLiteral("CARD-3"),
			QStringLiteral("PAIR_TOO_MANY"),
			QString(),
			QString()));
	if (!ok)
		return {};

	const QString path = out_dir.filePath(QStringLiteral("mam_continuation_pair_cardinality.qet"));
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
				QStringLiteral("mam_continuation_id"),
				QStringLiteral("mam_pair_id"),
				QStringLiteral("mam_chain_id"),
				QStringLiteral("mam_chain_order"),
				QStringLiteral("suggested_mam_pair_id"),
				QStringLiteral("migration_recommendation"),
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
		QVERIFY(!l1_out.value(QStringLiteral("suggested_mam_pair_id")).isEmpty());
		QCOMPARE(l1_out.value(QStringLiteral("migration_recommendation")), QStringLiteral("candidate: assign mam_pair_id"));

		const QHash<QString, QString> l1_in = rowByPotentialAndDirection(
			rows,
			QStringLiteral("1L1"),
			QStringLiteral("left"));
		QVERIFY(!l1_in.isEmpty());
		QCOMPARE(l1_in.value(QStringLiteral("status")), QStringLiteral("WARNING"));
		QCOMPARE(l1_in.value(QStringLiteral("computed_xref")), QStringLiteral("/1.5"));
		QCOMPARE(l1_in.value(QStringLiteral("visible_xref")), QStringLiteral("/1.0"));
		QCOMPARE(
			l1_in.value(QStringLiteral("suggested_mam_pair_id")),
			l1_out.value(QStringLiteral("suggested_mam_pair_id")));
		QCOMPARE(l1_in.value(QStringLiteral("migration_recommendation")), QStringLiteral("candidate: assign mam_pair_id"));
		QVERIFY(l1_in.value(QStringLiteral("diagnostics")).contains(QStringLiteral("stale visible xref")));

		const QHash<QString, QString> n1 = rowByPotentialAndDirection(
			rows,
			QStringLiteral("1N"),
			QStringLiteral("down"));
		QVERIFY(!n1.isEmpty());
		QCOMPARE(n1.value(QStringLiteral("status")), QStringLiteral("ERROR"));
		QCOMPARE(n1.value(QStringLiteral("cardinality")), QStringLiteral("unresolved"));
		QVERIFY(n1.value(QStringLiteral("diagnostics")).contains(QStringLiteral("missing continuation counterpart")));
		verifyNoMigrationSuggestion(n1);
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
		verifyNoMigrationSuggestion(empty_potential);

		const QList<QHash<QString, QString>> ambiguous_l1 = rowsByPotential(rows, QStringLiteral("1L1"));
		QCOMPARE(ambiguous_l1.size(), 3);
		for (const QHash<QString, QString> &row : ambiguous_l1) {
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("WARNING"));
			QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("ambiguous"));
			QVERIFY(row.value(QStringLiteral("diagnostics")).contains(QStringLiteral("ambiguous continuation group without chain order")));
			verifyNoMigrationSuggestion(row);
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
			verifyNoMigrationSuggestion(row);
		}
	}

	void potentialContinuationMigrationSkipsConflictingLegacyLabels()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString conflicting_fixture = writeContinuationConflictingSignalFixture(fixture, out_dir);
		QVERIFY2(!conflicting_fixture.isEmpty(), "failed to prepare conflicting-signal continuation fixture");

		const QString export_path = out_dir.filePath(QStringLiteral("mam_continuation_conflicting_signal.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-continuation"),
			conflicting_fixture,
			export_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(result.exit_code == 0,
				 qPrintable(QStringLiteral("conflicting-signal continuation export failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(result.exit_code)
								.arg(result.stdout_text.left(500))
								.arg(result.stderr_text.left(500))));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));

		const QList<QHash<QString, QString>> conflicting_signal = rowsBySignal(
			rows,
			QStringLiteral("SIG-CONFLICT"));
		QCOMPARE(conflicting_signal.size(), 2);
		for (const QHash<QString, QString> &row : conflicting_signal) {
			QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("1:1"));
			QCOMPARE(row.value(QStringLiteral("relationship")), QStringLiteral("auto-point-to-point"));
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("WARNING"));
			QVERIFY(row.value(QStringLiteral("diagnostics")).contains(QStringLiteral("conflicting legacy potential or signal labels")));
			verifyNoMigrationSuggestion(row);
		}
	}

	void potentialContinuationApplyPairIdsWritesSafeSuggestedPairs()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QList<QStringList> before_rows = exportMamContinuationRows(
			fixture,
			out_dir,
			QStringLiteral("mam_continuation_before_apply.csv"));
		QVERIFY(!before_rows.isEmpty());

		QSet<QString> suggested_pair_ids;
		int candidate_rows = 0;
		for (const QStringList &fields : before_rows.mid(1)) {
			const QStringList header = before_rows.first();
			const QString recommendation = fields.value(header.indexOf(QStringLiteral("migration_recommendation")));
			if (recommendation != QStringLiteral("candidate: assign mam_pair_id"))
				continue;
			++candidate_rows;
			suggested_pair_ids.insert(fields.value(header.indexOf(QStringLiteral("suggested_mam_pair_id"))));
		}
		QCOMPARE(candidate_rows, 6);
		QCOMPARE(suggested_pair_ids.size(), 3);

		const QString applied_path = out_dir.filePath(QStringLiteral("mam_continuation_pair_ids_applied.qet"));
		const CliTestUtils::CliResult apply = CliTestUtils::runQetCli({
			QStringLiteral("--apply-mam-continuation-pair-ids"),
			fixture,
			applied_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(apply.exit_code == 0,
				 qPrintable(QStringLiteral("continuation pair-id apply failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(apply.exit_code)
								.arg(apply.stdout_text.left(500))
								.arg(apply.stderr_text.left(500))));
		QVERIFY(apply.stdout_text.contains(QStringLiteral("pairs=3, elements=6")));
		QVERIFY(QFile::exists(applied_path));
		QVERIFY(QFileInfo(applied_path).size() > 0);

		const QList<QStringList> after_rows = exportMamContinuationRows(
			applied_path,
			out_dir,
			QStringLiteral("mam_continuation_after_apply.csv"));
		QVERIFY(!after_rows.isEmpty());
		QCOMPARE(after_rows.size(), before_rows.size());
		for (const QString &pair_id : suggested_pair_ids) {
			const QList<QHash<QString, QString>> applied_pair = rowsByMamPairId(after_rows, pair_id);
			QCOMPARE(applied_pair.size(), 2);
			for (const QHash<QString, QString> &row : applied_pair) {
				QCOMPARE(row.value(QStringLiteral("relationship")), QStringLiteral("mam_pair %1").arg(pair_id));
				QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("1:1"));
				verifyNoMigrationSuggestion(row);
			}
			QVERIFY(rowsBySuggestedMamPairId(after_rows, pair_id).isEmpty());
		}

		const QList<QHash<QString, QString>> unresolved_neutral = rowsByPotential(after_rows, QStringLiteral("1N"));
		QCOMPARE(unresolved_neutral.size(), 1);
		QVERIFY(unresolved_neutral.first().value(QStringLiteral("mam_pair_id")).isEmpty());
		QCOMPARE(unresolved_neutral.first().value(QStringLiteral("status")), QStringLiteral("ERROR"));
	}

	void potentialContinuationApplyPairIdsSkipsConflictingLegacyLabels()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString conflicting_fixture = writeContinuationConflictingSignalFixture(fixture, out_dir);
		QVERIFY2(!conflicting_fixture.isEmpty(), "failed to prepare conflicting-signal continuation fixture");

		const QString applied_path = out_dir.filePath(QStringLiteral("mam_continuation_conflicting_signal_applied.qet"));
		const CliTestUtils::CliResult apply = CliTestUtils::runQetCli({
			QStringLiteral("--apply-mam-continuation-pair-ids"),
			conflicting_fixture,
			applied_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(apply.exit_code == 0,
				 qPrintable(QStringLiteral("conflicting-signal continuation apply failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(apply.exit_code)
								.arg(apply.stdout_text.left(500))
								.arg(apply.stderr_text.left(500))));

		const QList<QStringList> after_rows = exportMamContinuationRows(
			applied_path,
			out_dir,
			QStringLiteral("mam_continuation_conflicting_signal_after_apply.csv"));
		QVERIFY(!after_rows.isEmpty());
		const QList<QHash<QString, QString>> conflicting_signal = rowsBySignal(
			after_rows,
			QStringLiteral("SIG-CONFLICT"));
		QCOMPARE(conflicting_signal.size(), 2);
		for (const QHash<QString, QString> &row : conflicting_signal) {
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("WARNING"));
			QVERIFY(row.value(QStringLiteral("mam_pair_id")).isEmpty());
			QVERIFY(row.value(QStringLiteral("diagnostics")).contains(QStringLiteral("conflicting legacy potential or signal labels")));
			verifyNoMigrationSuggestion(row);
		}
	}

	void potentialContinuationDiagnosticsReportChainOrderIssues()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString chain_fixture = writeContinuationChainDiagnosticsFixture(fixture, out_dir);
		QVERIFY2(!chain_fixture.isEmpty(), "failed to prepare chain-case continuation fixture");

		const QString export_path = out_dir.filePath(QStringLiteral("mam_continuation_chain.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-continuation"),
			chain_fixture,
			export_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(result.exit_code == 0,
				 qPrintable(QStringLiteral("chain continuation export failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(result.exit_code)
								.arg(result.stdout_text.left(500))
								.arg(result.stderr_text.left(500))));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 9);

		const QList<QHash<QString, QString>> ok_chain = rowsByChain(rows, QStringLiteral("CHAIN_OK"));
		QCOMPARE(ok_chain.size(), 3);
		for (const QHash<QString, QString> &row : ok_chain) {
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("OK"));
			QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("chain"));
			QVERIFY(row.value(QStringLiteral("relationship")).startsWith(QStringLiteral("chain CHAIN_OK order ")));
			QVERIFY(!row.value(QStringLiteral("computed_xref")).isEmpty());
			QVERIFY(row.value(QStringLiteral("diagnostics")).isEmpty());
			verifyNoMigrationSuggestion(row);
		}

		const QList<QHash<QString, QString>> bad_chain = rowsByChain(rows, QStringLiteral("CHAIN_BAD"));
		QCOMPARE(bad_chain.size(), 5);
		bool saw_missing_order = false;
		bool saw_duplicate_order = false;
		bool saw_missing_target = false;
		for (const QHash<QString, QString> &row : bad_chain) {
			const QString diagnostics = row.value(QStringLiteral("diagnostics"));
			saw_missing_order = saw_missing_order
				|| diagnostics.contains(QStringLiteral("missing chain_order"));
			saw_duplicate_order = saw_duplicate_order
				|| diagnostics.contains(QStringLiteral("duplicate chain_order 0"));
			saw_missing_target = saw_missing_target
				|| diagnostics.contains(QStringLiteral("missing chain target for direction"));
		}
		QVERIFY2(saw_missing_order, "expected missing chain_order diagnostic");
		QVERIFY2(saw_duplicate_order, "expected duplicate chain_order diagnostic");
		QVERIFY2(saw_missing_target, "expected missing chain target diagnostic");
	}

	void potentialContinuationDiagnosticsReadNamespacedPairFields()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString namespaced_fixture = writeContinuationNamespacedPairDiagnosticsFixture(fixture, out_dir);
		QVERIFY2(!namespaced_fixture.isEmpty(), "failed to prepare namespaced pair continuation fixture");

		const QString export_path = out_dir.filePath(QStringLiteral("mam_continuation_namespaced_pair.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-continuation"),
			namespaced_fixture,
			export_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(result.exit_code == 0,
				 qPrintable(QStringLiteral("namespaced continuation export failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(result.exit_code)
								.arg(result.stdout_text.left(500))
								.arg(result.stderr_text.left(500))));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 9);

		const QList<QHash<QString, QString>> pair_ok = rowsByMamPairId(rows, QStringLiteral("PAIR_OK"));
		QCOMPARE(pair_ok.size(), 2);
		for (const QHash<QString, QString> &row : pair_ok) {
			QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("1:1"));
			QCOMPARE(row.value(QStringLiteral("relationship")), QStringLiteral("mam_pair PAIR_OK"));
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("OK"));
			QVERIFY(row.value(QStringLiteral("diagnostics")).isEmpty());
			verifyNoMigrationSuggestion(row);
		}

		const QList<QHash<QString, QString>> single_pair = rowsByMamPairId(rows, QStringLiteral("PAIR_SINGLE"));
		QCOMPARE(single_pair.size(), 1);
		QCOMPARE(single_pair.first().value(QStringLiteral("status")), QStringLiteral("ERROR"));
		QCOMPARE(single_pair.first().value(QStringLiteral("cardinality")), QStringLiteral("unresolved"));
		QVERIFY(single_pair.first().value(QStringLiteral("diagnostics")).contains(QStringLiteral("mam_pair_id expects exactly two continuations")));

		const QList<QHash<QString, QString>> triple_pair = rowsByMamPairId(rows, QStringLiteral("PAIR_THREE"));
		QCOMPARE(triple_pair.size(), 3);
		for (const QHash<QString, QString> &row : triple_pair) {
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("ERROR"));
			QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("invalid-pair"));
			QCOMPARE(row.value(QStringLiteral("relationship")), QStringLiteral("mam_pair PAIR_THREE"));
			QVERIFY(row.value(QStringLiteral("diagnostics")).contains(QStringLiteral("mam_pair_id expects exactly two continuations")));
		}

		const QList<QHash<QString, QString>> conflicting_pair = rowsByMamPairId(rows, QStringLiteral("PAIR_CHAIN"));
		QCOMPARE(conflicting_pair.size(), 2);
		for (const QHash<QString, QString> &row : conflicting_pair) {
			QCOMPARE(row.value(QStringLiteral("mam_chain_id")), QStringLiteral("CHAIN_CONFLICT"));
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("ERROR"));
			QVERIFY(row.value(QStringLiteral("diagnostics")).contains(QStringLiteral("mam_pair_id cannot be combined")));
		}
	}

	void potentialContinuationApplyPairIdsSkipsChains()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString chain_fixture = writeContinuationChainDiagnosticsFixture(fixture, out_dir);
		QVERIFY2(!chain_fixture.isEmpty(), "failed to prepare chain-case continuation fixture");

		const QString applied_path = out_dir.filePath(QStringLiteral("mam_continuation_chain_applied.qet"));
		const CliTestUtils::CliResult apply = CliTestUtils::runQetCli({
			QStringLiteral("--apply-mam-continuation-pair-ids"),
			chain_fixture,
			applied_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(apply.exit_code == 0,
				 qPrintable(QStringLiteral("chain continuation apply failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(apply.exit_code)
								.arg(apply.stdout_text.left(500))
								.arg(apply.stderr_text.left(500))));

		const QList<QStringList> after_rows = exportMamContinuationRows(
			applied_path,
			out_dir,
			QStringLiteral("mam_continuation_chain_after_apply.csv"));
		QVERIFY(!after_rows.isEmpty());
		const QList<QHash<QString, QString>> ok_chain = rowsByChain(after_rows, QStringLiteral("CHAIN_OK"));
		QCOMPARE(ok_chain.size(), 3);
		for (const QHash<QString, QString> &row : ok_chain) {
			QVERIFY(row.value(QStringLiteral("mam_pair_id")).isEmpty());
			QVERIFY(row.value(QStringLiteral("relationship")).startsWith(QStringLiteral("chain CHAIN_OK order ")));
			verifyNoMigrationSuggestion(row);
		}
	}

	void potentialContinuationDiagnosticsReadNamespacedChainFields()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString namespaced_fixture = writeContinuationNamespacedChainDiagnosticsFixture(fixture, out_dir);
		QVERIFY2(!namespaced_fixture.isEmpty(), "failed to prepare namespaced chain continuation fixture");

		const QString export_path = out_dir.filePath(QStringLiteral("mam_continuation_namespaced_chain.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-continuation"),
			namespaced_fixture,
			export_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(result.exit_code == 0,
				 qPrintable(QStringLiteral("namespaced chain continuation export failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(result.exit_code)
								.arg(result.stdout_text.left(500))
								.arg(result.stderr_text.left(500))));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		QCOMPARE(rows.size(), 9);

		const QList<QHash<QString, QString>> mam_chain = rowsByChain(rows, QStringLiteral("MAM_CHAIN_OK"));
		QCOMPARE(mam_chain.size(), 3);
		for (const QHash<QString, QString> &row : mam_chain) {
			QCOMPARE(row.value(QStringLiteral("mam_chain_id")), QStringLiteral("MAM_CHAIN_OK"));
			QVERIFY(!row.value(QStringLiteral("mam_chain_order")).isEmpty());
			QCOMPARE(row.value(QStringLiteral("chain")), QStringLiteral("MAM_CHAIN_OK"));
			QCOMPARE(row.value(QStringLiteral("chain_order")), row.value(QStringLiteral("mam_chain_order")));
			QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("chain"));
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("OK"));
			QVERIFY(row.value(QStringLiteral("relationship")).startsWith(QStringLiteral("mam_chain MAM_CHAIN_OK order ")));
			QVERIFY(row.value(QStringLiteral("diagnostics")).isEmpty());
		}
	}

	void potentialContinuationDiagnosticsRejectsPairCardinalityAboveTwo()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString pair_fixture = writeContinuationPairCardinalityFixture(fixture, out_dir);
		QVERIFY2(!pair_fixture.isEmpty(), "failed to prepare pair cardinality fixture");

		const QString export_path = out_dir.filePath(QStringLiteral("mam_continuation_pair_cardinality.csv"));
		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-continuation"),
			pair_fixture,
			export_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(result.exit_code == 0,
				 qPrintable(QStringLiteral("pair-cardinality continuation export failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(result.exit_code)
								.arg(result.stdout_text.left(500))
								.arg(result.stderr_text.left(500))));

		const QList<QStringList> rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(export_path)));
		const QList<QHash<QString, QString>> bad_pair = rowsByMamPairId(rows, QStringLiteral("PAIR_TOO_MANY"));
		QCOMPARE(bad_pair.size(), 3);
		for (const QHash<QString, QString> &row : bad_pair) {
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("ERROR"));
			QCOMPARE(row.value(QStringLiteral("cardinality")), QStringLiteral("invalid-pair"));
			QCOMPARE(row.value(QStringLiteral("relationship")), QStringLiteral("mam_pair PAIR_TOO_MANY"));
			QVERIFY(row.value(QStringLiteral("diagnostics")).contains(QStringLiteral("mam_pair_id expects exactly two continuations")));
		}
	}

	void potentialContinuationNamespacedFieldsSurviveResave()
	{
		const QString fixture = currentPathContinuationExample();
		QVERIFY2(!fixture.isEmpty(), "MAM current-path continuation example project not found");

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());
		const QString namespaced_fixture = writeContinuationNamespacedPairDiagnosticsFixture(fixture, out_dir);
		QVERIFY2(!namespaced_fixture.isEmpty(), "failed to prepare namespaced continuation fixture");

		const QString original_export_path = out_dir.filePath(QStringLiteral("mam_continuation_before_resave.csv"));
		const CliTestUtils::CliResult original_export = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-continuation"),
			namespaced_fixture,
			original_export_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(original_export.exit_code == 0,
				 qPrintable(QStringLiteral("initial continuation export failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(original_export.exit_code)
								.arg(original_export.stdout_text.left(500))
								.arg(original_export.stderr_text.left(500))));

		const QString resaved_path = out_dir.filePath(QStringLiteral("mam_continuation_namespaced_resaved.qet"));
		const CliTestUtils::CliResult resave = CliTestUtils::runQetCli({
			QStringLiteral("--resave"),
			namespaced_fixture,
			resaved_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(resave.exit_code == 0,
				 qPrintable(QStringLiteral("namespaced continuation resave failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(resave.exit_code)
								.arg(resave.stdout_text.left(500))
								.arg(resave.stderr_text.left(500))));
		QVERIFY(QFile::exists(resaved_path));
		QVERIFY(QFileInfo(resaved_path).size() > 0);

		const QString resaved_export_path = out_dir.filePath(QStringLiteral("mam_continuation_after_resave.csv"));
		const CliTestUtils::CliResult resaved_export = CliTestUtils::runQetCli({
			QStringLiteral("--export-mam-continuation"),
			resaved_path,
			resaved_export_path
		}, 30000, continuationCliUsesOffscreenPlatform());
		QVERIFY2(resaved_export.exit_code == 0,
				 qPrintable(QStringLiteral("resaved continuation export failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(resaved_export.exit_code)
								.arg(resaved_export.stdout_text.left(500))
								.arg(resaved_export.stderr_text.left(500))));

		const QList<QStringList> original_rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(original_export_path)));
		const QList<QStringList> resaved_rows = CliTestUtils::parseSemicolonCsv(
			QString::fromUtf8(CliTestUtils::readFile(resaved_export_path)));
		QCOMPARE(resaved_rows, original_rows);

		const QList<QHash<QString, QString>> pair_ok = rowsByMamPairId(
			resaved_rows,
			QStringLiteral("PAIR_OK"));
		QCOMPARE(pair_ok.size(), 2);
		for (const QHash<QString, QString> &row : pair_ok) {
			QVERIFY(row.value(QStringLiteral("mam_continuation_id")).startsWith(QStringLiteral("CONT-")));
			QCOMPARE(row.value(QStringLiteral("relationship")), QStringLiteral("mam_pair PAIR_OK"));
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("OK"));
		}

		const QList<QHash<QString, QString>> conflicting_pair = rowsByMamPairId(
			resaved_rows,
			QStringLiteral("PAIR_CHAIN"));
		QCOMPARE(conflicting_pair.size(), 2);
		for (const QHash<QString, QString> &row : conflicting_pair) {
			QCOMPARE(row.value(QStringLiteral("mam_chain_id")), QStringLiteral("CHAIN_CONFLICT"));
			QVERIFY(!row.value(QStringLiteral("mam_chain_order")).isEmpty());
			QCOMPARE(row.value(QStringLiteral("status")), QStringLiteral("ERROR"));
		}
	}
};

QTEST_APPLESS_MAIN(tst_cli_export_equivalence)

#include "tst_cli_export_equivalence.moc"

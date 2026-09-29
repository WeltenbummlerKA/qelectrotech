#include <QtTest>

#include "cli_test_utils.h"

#include <QDomDocument>
#include <QFile>
#include <QTemporaryDir>

namespace {

QDomDocument loadXml(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return {};

	QDomDocument doc;
	const QDomDocument::ParseResult result = doc.setContent(&file);
	if (!result) {
		qWarning().noquote()
			<< QStringLiteral("XML parse error in %1:%2:%3: %4")
				  .arg(path)
				  .arg(result.errorLine)
				  .arg(result.errorColumn)
				  .arg(result.errorMessage);
		return {};
	}
	return doc;
}

bool writeXml(const QDomDocument &doc, const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;
	QTextStream stream(&file);
	stream << doc.toString(4);
	return true;
}

QDomElement firstElementByUuid(const QDomDocument &doc, const QString &uuid)
{
	const QDomNodeList elements = doc.elementsByTagName(QStringLiteral("element"));
	for (int i = 0; i < elements.count(); ++i) {
		const QDomElement element = elements.at(i).toElement();
		if (element.attribute(QStringLiteral("uuid")) == uuid)
			return element;
	}
	return {};
}

void setElementInformation(QDomDocument &doc, const QString &uuid, const QString &name, const QString &value)
{
	QDomElement element = firstElementByUuid(doc, uuid);
	QVERIFY2(!element.isNull(), qPrintable(QStringLiteral("missing element %1").arg(uuid)));

	QDomElement informations = element.firstChildElement(QStringLiteral("elementInformations"));
	if (informations.isNull()) {
		informations = doc.createElement(QStringLiteral("elementInformations"));
		const QDomElement properties = element.firstChildElement(QStringLiteral("properties"));
		if (properties.isNull())
			element.appendChild(informations);
		else
			element.insertBefore(informations, properties);
	}

	QDomElement information;
	for (QDomElement candidate = informations.firstChildElement(QStringLiteral("elementInformation"));
		 !candidate.isNull();
		 candidate = candidate.nextSiblingElement(QStringLiteral("elementInformation"))) {
		if (candidate.attribute(QStringLiteral("name")) == name) {
			information = candidate;
			break;
		}
	}

	if (information.isNull()) {
		information = doc.createElement(QStringLiteral("elementInformation"));
		information.setAttribute(QStringLiteral("show"), QStringLiteral("1"));
		information.setAttribute(QStringLiteral("name"), name);
		informations.appendChild(information);
	}

	while (information.hasChildNodes())
		information.removeChild(information.firstChild());
	information.appendChild(doc.createTextNode(value));
}

QDomElement onlyTerminalStrip(const QDomDocument &doc)
{
	const QDomNodeList strips = doc.elementsByTagName(QStringLiteral("terminal_strip"));
	if (strips.count() != 1)
		return {};
	return strips.at(0).toElement();
}

QString terminalStripName(const QDomElement &strip)
{
	const QDomElement informations =
		strip.firstChildElement(QStringLiteral("terminal_strip_data"))
			.firstChildElement(QStringLiteral("informations"));
	for (QDomElement information = informations.firstChildElement(QStringLiteral("information"));
		 !information.isNull();
		 information = information.nextSiblingElement(QStringLiteral("information"))) {
		if (information.attribute(QStringLiteral("name")) == QStringLiteral("name"))
			return information.text();
	}
	return {};
}

QVector<QStringList> physicalTerminalUuids(const QDomElement &strip)
{
	QVector<QStringList> physical_terminals;
	const QDomElement layout = strip.firstChildElement(QStringLiteral("layout"));
	for (QDomElement physical = layout.firstChildElement(QStringLiteral("physical_terminal"));
		 !physical.isNull();
		 physical = physical.nextSiblingElement(QStringLiteral("physical_terminal"))) {
		QStringList real_terminal_uuids;
		for (QDomElement real = physical.firstChildElement(QStringLiteral("real_terminal"));
			 !real.isNull();
			 real = real.nextSiblingElement(QStringLiteral("real_terminal"))) {
			real_terminal_uuids << real.attribute(QStringLiteral("element_uuid"));
		}
		physical_terminals.append(real_terminal_uuids);
	}
	return physical_terminals;
}

} // namespace

class tst_terminalstrip_assignment : public QObject
{
	Q_OBJECT

private slots:
	void assignsFreeSingleLevelTerminalsByPrefixInNaturalOrder()
	{
		const QString fixture = QFINDTESTDATA("fixtures/terminal_strip_synthetic_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "synthetic terminal-strip fixture project not found");

		QDomDocument doc = loadXml(fixture);
		QVERIFY2(!doc.isNull(), "fixture XML is not parseable");

		const QDomNodeList terminal_strip_sections =
			doc.documentElement().elementsByTagName(QStringLiteral("terminal_strips"));
		QCOMPARE(terminal_strip_sections.count(), 1);
		doc.documentElement().removeChild(terminal_strip_sections.at(0));

		setElementInformation(doc, QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa1}"),
							  QStringLiteral("label"), QStringLiteral("-X1:10"));
		setElementInformation(doc, QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa2}"),
							  QStringLiteral("label"), QStringLiteral("-X1:2"));
		setElementInformation(doc, QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa3}"),
							  QStringLiteral("label"), QStringLiteral("-X1:1"));
		setElementInformation(doc, QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa4}"),
							  QStringLiteral("label"), QStringLiteral("-X2:1"));

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		const QString input = dir.filePath(QStringLiteral("assignment_input.qet"));
		const QString output = dir.filePath(QStringLiteral("assignment_output.qet"));
		QVERIFY2(writeXml(doc, input), "prepared assignment fixture was not written");

		const CliTestUtils::CliResult result = CliTestUtils::runQetCli({
			QStringLiteral("--assign-terminal-strip"),
			input,
			output,
			QStringLiteral("-X1")
		});
		QVERIFY2(result.exit_code == 0,
				 qPrintable(QStringLiteral("assignment failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(result.exit_code)
								.arg(result.stdout_text.left(500))
								.arg(result.stderr_text.left(500))));
		QVERIFY(result.stdout_text.contains(QStringLiteral("created=true")));
		QVERIFY(result.stdout_text.contains(QStringLiteral("added=3")));
		QVERIFY2(QFile::exists(output), "assignment output was not created");

		const QDomDocument output_doc = loadXml(output);
		QVERIFY2(!output_doc.isNull(), "assignment output XML is not parseable");
		const QDomElement strip = onlyTerminalStrip(output_doc);
		QVERIFY2(!strip.isNull(), "exactly one terminal strip should be created");
		QCOMPARE(terminalStripName(strip), QStringLiteral("-X1"));

		const QVector<QStringList> physical_terminals = physicalTerminalUuids(strip);
		QCOMPARE(physical_terminals.size(), 3);
		QCOMPARE(physical_terminals.at(0),
				 QStringList({QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa3}")}));
		QCOMPARE(physical_terminals.at(1),
				 QStringList({QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa2}")}));
		QCOMPARE(physical_terminals.at(2),
				 QStringList({QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa1}")}));
		QVERIFY2(strip.firstChildElement(QStringLiteral("terminal_strip_bridge")).isNull(),
				 "single-level assignment must not create bridges");
	}
};

QTEST_APPLESS_MAIN(tst_terminalstrip_assignment)

#include "tst_terminalstrip_assignment.moc"

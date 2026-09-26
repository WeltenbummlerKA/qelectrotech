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

QString canonicalNode(const QDomNode &node)
{
	if (node.isText())
		return node.nodeValue().simplified();

	if (!node.isElement())
		return {};

	const QDomElement element = node.toElement();
	QString out = QStringLiteral("<") + element.tagName();

	QStringList attributes;
	const QDomNamedNodeMap map = element.attributes();
	for (int i = 0; i < map.count(); ++i) {
		const QDomNode attr = map.item(i);
		attributes << attr.nodeName() + QStringLiteral("=") + attr.nodeValue();
	}
	attributes.sort();
	for (const QString &attribute : attributes)
		out += QStringLiteral(" ") + attribute;
	out += QStringLiteral(">");

	for (QDomNode child = element.firstChild(); !child.isNull(); child = child.nextSibling()) {
		const QString canonical_child = canonicalNode(child);
		if (!canonical_child.isEmpty())
			out += canonical_child;
	}

	out += QStringLiteral("</") + element.tagName() + QStringLiteral(">");
	return out;
}

QDomElement onlyChildElement(const QDomElement &parent, const QString &tag_name)
{
	const QDomNodeList nodes = parent.elementsByTagName(tag_name);
	if (nodes.count() != 1)
		return {};
	return nodes.at(0).toElement();
}

QStringList terminalStripRealTerminalUuids(const QDomElement &terminal_strip)
{
	QStringList uuids;
	const QDomElement layout = terminal_strip.firstChildElement(QStringLiteral("layout"));
	for (QDomElement physical = layout.firstChildElement(QStringLiteral("physical_terminal"));
		 !physical.isNull();
		 physical = physical.nextSiblingElement(QStringLiteral("physical_terminal"))) {
		for (QDomElement real = physical.firstChildElement(QStringLiteral("real_terminal"));
			 !real.isNull();
			 real = real.nextSiblingElement(QStringLiteral("real_terminal"))) {
			uuids << real.attribute(QStringLiteral("element_uuid"));
		}
	}
	return uuids;
}

void verifySyntheticTerminalStrip(const QDomDocument &doc)
{
	QVERIFY2(!doc.isNull(), "project XML is not parseable");
	QCOMPARE(doc.documentElement().tagName(), QStringLiteral("project"));

	const QDomElement terminal_strips = onlyChildElement(doc.documentElement(), QStringLiteral("terminal_strips"));
	QVERIFY2(!terminal_strips.isNull(), "terminal_strips section should be present");

	const QDomNodeList strip_nodes = terminal_strips.elementsByTagName(QStringLiteral("terminal_strip"));
	QCOMPARE(strip_nodes.count(), 1);
	const QDomElement strip = strip_nodes.at(0).toElement();

	const QDomElement data = strip.firstChildElement(QStringLiteral("terminal_strip_data"));
	QCOMPARE(data.attribute(QStringLiteral("uuid")), QStringLiteral("{bbbbbbbb-bbbb-4bbb-8bbb-bbbbbbbbbbbb}"));

	const QStringList real_terminal_uuids = terminalStripRealTerminalUuids(strip);
	QCOMPARE(real_terminal_uuids,
			 QStringList({QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa1}"),
						  QStringLiteral("{aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaa2}")}));

	const QDomElement bridge = strip.firstChildElement(QStringLiteral("terminal_strip_bridge"));
	QVERIFY2(!bridge.isNull(), "terminal-strip bridge should be present");
	QCOMPARE(bridge.attribute(QStringLiteral("uuid")), QStringLiteral("{cccccccc-cccc-4ccc-8ccc-cccccccccccc}"));
	QCOMPARE(bridge.attribute(QStringLiteral("color")), QStringLiteral("#ff8800"));

	QStringList bridge_uuids;
	const QDomElement bridge_terminals = bridge.firstChildElement(QStringLiteral("real_terminals"));
	for (QDomElement real = bridge_terminals.firstChildElement(QStringLiteral("real_terminal"));
		 !real.isNull();
		 real = real.nextSiblingElement(QStringLiteral("real_terminal"))) {
		bridge_uuids << real.attribute(QStringLiteral("uuid"));
	}
	QCOMPARE(bridge_uuids, real_terminal_uuids);
}

} // namespace

class tst_terminalstrip_roundtrip : public QObject
{
	Q_OBJECT

private slots:
	void syntheticTerminalStripResavePreservesMembershipAndBridge()
	{
		const QString binary = QStringLiteral(QET_TEST_BINARY_PATH);
		QVERIFY2(QFile::exists(binary),
				 qPrintable(QStringLiteral("qelectrotech binary not found at '%1'").arg(binary)));

		const QString fixture = QFINDTESTDATA("fixtures/terminal_strip_synthetic_minimal.qet");
		QVERIFY2(!fixture.isEmpty(), "synthetic terminal-strip fixture project not found");
		verifySyntheticTerminalStrip(loadXml(fixture));

		QTemporaryDir out_dir;
		QVERIFY(out_dir.isValid());

		const QString first = out_dir.filePath(QStringLiteral("first.qet"));
		const QString second = out_dir.filePath(QStringLiteral("second.qet"));

		const CliTestUtils::CliResult first_run =
			CliTestUtils::runQetCli({QStringLiteral("--resave"), fixture, first});
		QVERIFY2(first_run.exit_code == 0,
				 qPrintable(QStringLiteral("first --resave failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(first_run.exit_code)
								.arg(first_run.stdout_text.left(500))
								.arg(first_run.stderr_text.left(500))));
		QVERIFY2(QFile::exists(first), "first resave output was not created");

		const CliTestUtils::CliResult second_run =
			CliTestUtils::runQetCli({QStringLiteral("--resave"), first, second});
		QVERIFY2(second_run.exit_code == 0,
				 qPrintable(QStringLiteral("second --resave failed with exit %1\nstdout: %2\nstderr: %3")
								.arg(second_run.exit_code)
								.arg(second_run.stdout_text.left(500))
								.arg(second_run.stderr_text.left(500))));
		QVERIFY2(QFile::exists(second), "second resave output was not created");

		const QDomDocument first_doc = loadXml(first);
		const QDomDocument second_doc = loadXml(second);
		verifySyntheticTerminalStrip(first_doc);
		verifySyntheticTerminalStrip(second_doc);
		QCOMPARE(canonicalNode(second_doc.documentElement()), canonicalNode(first_doc.documentElement()));
	}
};

QTEST_APPLESS_MAIN(tst_terminalstrip_roundtrip)

#include "tst_terminalstrip_roundtrip.moc"

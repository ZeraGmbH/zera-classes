#include "main-unittest-qt.h"
#include "qt-unittest-xmlconfigreadertest.h"
#include "xmlconfigreader.h"

static QObject* pSourceIoTest = addTest(new XMLConfigReaderTest);

static const QString xmlPath = QStringLiteral(SOURCEPATH) + QStringLiteral("/tests/xmls/");
static const QString xsdPath = QStringLiteral(SOURCEPATH) + QStringLiteral("/tests/schemas/");

void XMLConfigReaderTest::schemaFound()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(xsdPath + "with-complex.xsd"));
}

void XMLConfigReaderTest::schemaFoundResource()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
}

void XMLConfigReaderTest::schemaNotFound()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(!reader.loadSchema(":/schemas/foo.xsd"));
}

void XMLConfigReaderTest::validPairSchemaXmlResourceFile()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QVERIFY(reader.loadXMLFile(":/xmls/with-complex-valid.xml"));
}

void XMLConfigReaderTest::validPairSchemaXmlFile()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QVERIFY(reader.loadXMLFile(xmlPath + "with-complex-valid.xml"));
}

void XMLConfigReaderTest::validPairSchemaXmlFileOld()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QVERIFY(reader.loadXML(xmlPath + "with-complex-valid.xml"));
}

void XMLConfigReaderTest::invalidNoSchema()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(!reader.loadXMLFile(":/xmls/with-complex-valid.xml"));
}

void XMLConfigReaderTest::invalidIP()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QVERIFY(!reader.loadXML(":/xmls/with-complex-invalid-ip.xml"));
}

void XMLConfigReaderTest::invalidYesNo()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QVERIFY(!reader.loadXML(":/xmls/with-complex-invalid-yes-no.xml"));
}

void XMLConfigReaderTest::invalidKeySequence()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QVERIFY(!reader.loadXML(":/xmls/with-complex-invalid-sequence.xml"));
}

void XMLConfigReaderTest::setGetValidIp()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QVERIFY(reader.loadXMLFile(":/xmls/with-complex-valid.xml"));
    QString newIp = "192.168.2.5";
    QString xml = "testvals:ethernet:ip";
    QVERIFY(reader.setValue(xml, newIp));
    QCOMPARE(reader.getValue(xml), newIp);
}

void XMLConfigReaderTest::setGetValidYesNoShouldNotWorkOnInvalid()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QVERIFY(reader.loadXMLFile(":/xmls/with-complex-valid.xml"));
    QString newYesNoInvalid = "2";
    QString xml = "testvals:yesno";
    QVERIFY(reader.setValue(xml, newYesNoInvalid));
    QCOMPARE(reader.getValue(xml), newYesNoInvalid);
}

void XMLConfigReaderTest::setWithInvalidKey()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QVERIFY(reader.loadXMLFile(":/xmls/with-complex-valid.xml"));
    QString newYesNoInvalid = "0";
    QString xml = "testvals:foo";
    QVERIFY(!reader.setValue(xml, newYesNoInvalid));
}

void XMLConfigReaderTest::loadAndModifyExportSequenceOfFields()
{
    Zera::XMLConfig::cReader reader;
    QVERIFY(reader.loadSchema(":/schemas/with-complex.xsd"));
    QString xmlPath = ":/xmls/with-complex-valid.xml";
    QVERIFY(reader.loadXMLFile(xmlPath));
    QString oldIp = "1.2.3.4";
    QString newIp = "192.168.2.5";
    QString xml = "testvals:ethernet:ip";
    QVERIFY(reader.setValue(xml, newIp));

    QString xmlRead = reader.getXMLConfig().simplified().remove(' ');

    QFile xmlFile(xmlPath);
    xmlFile.open(QFile::ReadOnly);
    QString xmlOrig = QString(xmlFile.readAll()).simplified().remove(' ');
    QVERIFY(xmlRead != xmlOrig);

    QString xmlOrigManModify = xmlOrig.replace(oldIp, newIp);
    QCOMPARE(xmlRead, xmlOrigManModify);
}


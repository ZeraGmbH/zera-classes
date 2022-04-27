#include "main-unittest-qt.h"
#include "qt-unittest-xmlconfigreadertest.h"
#include "xmlconfigreader.h"

static QObject* pSourceIoTest = addTest(new XMLConfigReaderTest);

void XMLConfigReaderTest::schemaFound()
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


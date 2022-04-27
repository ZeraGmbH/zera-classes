#ifndef XMLCONFIGREADERTEST_H
#define XMLCONFIGREADERTEST_H

#include <QObject>

class XMLConfigReaderTest : public QObject
{
    Q_OBJECT
private slots:
    void schemaFound();
    void schemaFoundResource();
    void schemaNotFound();
    void validPairSchemaXmlResourceFile();
    void validPairSchemaXmlFile();
    void validPairSchemaXmlFileOld(); // As long as cReader::loadXML still exists
    void invalidNoSchema();

    void invalidIP();
    void invalidYesNo();
    void invalidKeySequence();

    void setGetValidIp();
    void setGetValidYesNoShouldNotWorkOnInvalid(); // how can we fix this???

    void setWithInvalidKey();

    void loadAndModifyExportSequenceOfFields();
};

#endif // XMLCONFIGREADERTEST_H

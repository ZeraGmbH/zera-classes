#ifndef XMLCONFIGREADERTEST_H
#define XMLCONFIGREADERTEST_H

#include <QObject>

class XMLConfigReaderTest : public QObject
{
    Q_OBJECT
private slots:
    void schemaFound();
    void schemaNotFound();
    void validPairSchemaXmlResourceFile();
    void validPairSchemaXmlFile();
    void validPairSchemaXmlFileOld(); // As long as cReader::loadXML still exists

};

#endif // XMLCONFIGREADERTEST_H

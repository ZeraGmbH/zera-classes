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


};

#endif // XMLCONFIGREADERTEST_H

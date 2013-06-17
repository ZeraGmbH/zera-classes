#ifndef XMLCONFIGREADER_PRIVATE_H
#define XMLCONFIGREADER_PRIVATE_H

#include <QObject>
#include <QHash>
#include <QVariant>


namespace Zera
{
  namespace XMLConfig
  {
    // class is private and hidden for the outside world, requires GCC>=4.0
    class __attribute__ ((visibility ("hidden"))) _XMLConfigReaderPrivate : public QObject
    {
      Q_OBJECT
      public:
      explicit _XMLConfigReaderPrivate(QObject *parent = 0);

      bool loadSchema(const QString &filePath);
      bool loadXML(const QString &filePath);

      QVariant getValue(const QString &key);
      bool setValue(const QString &key, QVariant value);

      QString getXMLConfig();


      signals:
      void valueChanged(const QString &key);
      void finishedParsingXML();

      private:
      QHash<QString, QVariant> schema2Config(const QString &xsdPath);
      bool xml2Config(const QString &xmlPath);

      QHash<QString, QVariant> data;
      QString schemaFilePath;
    };
  }
}
#endif // XMLCONFIGREADER_PRIVATE_H

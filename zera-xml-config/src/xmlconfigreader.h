#ifndef XMLCONFIGREADER_H
#define XMLCONFIGREADER_H

#include "zeraxmlconfig_export.h"
#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

namespace Zera
{
  namespace XMLConfig
  {

    class cReaderPrivate;

    /**
     * @brief The XMLConfigReader class
     * Reads XSD and XML files as configuration
     */
    class ZERAXMLCONFIG_EXPORT cReader : public QObject
    {
      Q_OBJECT

    public:
      /**
       * @brief Reader default constructor
       * @param parent
       */
      explicit cReader(QObject *parent = 0);

      /**
       * @brief Load a XSD schema file and initialize the configuration
       * @param filePath File to load
       * @return true for success
       */
      bool loadSchema(QString filePath);
      /**
       * @brief Loads a XML file and sets the configuration if it matches the schema file
       * @param filePath File to load
       * @return true for success
       */
      bool loadXML(QString filePath);
      bool loadXMLFile(QString path);

      /**
       * @brief Parses the QString and sets the configuration if the XML data matches the schema file
       * @param xmlString string to parse
       * @return true for success
       */
      bool loadXMLFromString(QString xmlString);

      /**
       * @brief Retrieves a value from the configuration map
       * @param key The map key where the value has been stored
       * @return QString value
       */
      QString getValue(QString key);

      /**
       * @brief Sets config values
       * @param key The configuration key in the QHash
       * @param value The new value
       *
       * @return true for success
       */
      bool setValue(QString key, QString value);

      /**
       * @brief Exports the QHash to a XML file format
       * @return XML code
       */
      QString getXMLConfig();


    signals:
      /**
       * @brief Notifies when values change
       * @param key The key that changed
       */
      void valueChanged(QString key);

      /**
       * @brief Loading the XML config file is complete
       */
      void finishedParsingXML(bool ok);

    protected:
      /**
       * @brief Opaque pointer for ABI safety
       */
      Zera::XMLConfig::cReaderPrivate *d_ptr;

    private:
      /**
       * @brief xml2Config
       * @param xmlData
       * @return true for success
       */
      bool xml2Config(QIODevice* xmlData);

      void parseLists(QList<QString> oldList, QList<QString> newList, QXmlStreamWriter &writer);

      Q_DISABLE_COPY(cReader)
      Q_DECLARE_PRIVATE(cReader)
    };
  }
}

#endif // XMLCONFIGREADER_H

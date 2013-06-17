#ifndef XMLCONFIGREADER_H
#define XMLCONFIGREADER_H

#include "xml_config_global.h"
#include <QObject>

namespace Zera
{
  namespace XMLConfig
  {

    class _XMLConfigReaderPrivate;

    /**
     * @brief The XMLConfigReader class
     * Reads XSD and XML files as configuration
     */
    class ZERA_XML_CONFIGSHARED_EXPORT XMLConfigReader : public QObject
    {
      Q_OBJECT

    protected:
      /**
       * @brief Opaque pointer for ABI safety
       */
      _XMLConfigReaderPrivate *d_ptr;

    public:
      explicit XMLConfigReader(QObject *parent = 0);
      ~XMLConfigReader();

      /**
       * @brief Load a XSD schema file and initialize the configuration
       * @param filePath File to load
       * @return true for success
       */
      bool loadSchema(const QString &filePath);
      /**
       * @brief Loads a XML file and sets the configuration if it matches the schema file
       * @param filePath File to load
       * @return true for success
       */
      bool loadXML(const QString &filePath);

      /**
       * @brief Retrieves a value from the configuration map
       * @param key The map key where the value has been stored
       * @return QVariant value
       */
      QVariant getValue(const QString &key);
      /**
       * @brief Sets config values
       * @param key The configuration key in the QMap
       * @param value The new value
       * @return true for success
       */
      bool setValue(const QString &key, QVariant value);

      /**
       * @brief Exports the QMap to a XML file format
       * @return XML code
       */
      QString getXMLConfig();


    signals:
      /**
       * @brief Notifies when values change
       * @param key The key that changed
       */
      void valueChanged(const QString &key);
      /**
       * @brief Loading the XML config file is complete
       */
      void finishedParsingXML();

    private:
    };
  }
}

#endif // XMLCONFIGREADER_H

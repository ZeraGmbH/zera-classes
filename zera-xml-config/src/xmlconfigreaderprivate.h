#ifndef XMLCONFIGREADER_PRIVATE_H
#define XMLCONFIGREADER_PRIVATE_H

#include <QHash>
#include <QVariant>


namespace Zera
{
  namespace XMLConfig
  {
    class cReader;

    /**
     * @brief Private data of Zera::XMLConfig::Reader for ABI safety
     */
    class cReaderPrivate
    {
    public:
      explicit cReaderPrivate();

    private:
      /**
       * @brief schema2Config
       * @param xsdPath
       * @return
       */
      QHash<QString, QVariant> schema2Config(const QString &xsdPath);
      /**
       * @brief xml2Config
       * @param xmlData
       * @return
       */
      bool xml2Config(QIODevice* xmlData);

      /**
       * @brief data Data member holding all config values
       */
      QHash<QString, QVariant> data;

      /**
       * @brief schemaFilePath cached path to the schema file
       */
      QString schemaFilePath;

      /**
       * @brief q_ptr See Q_DECLARE_PUBLIC
       */
      Zera::XMLConfig::cReader *q_ptr;

      Q_DECLARE_PUBLIC(cReader)
    };
  }
}
#endif // XMLCONFIGREADER_PRIVATE_H

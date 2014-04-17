#ifndef XMLCONFIGREADER_PRIVATE_H
#define XMLCONFIGREADER_PRIVATE_H

#include "messagehandler.h"

#include <QHash>
#include <QString>

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
      /**
       * @brief cReaderPrivate Default constructor
       * @param parent the q_ptr of this instance
       */
      cReaderPrivate(cReader *parent);

    private:

      /**
       * @brief data Data member holding all config values
       */
      QHash<QString, QString> data;

      /**
       * @brief schemaFilePath cached path to the schema file
       */
      QString schemaFilePath;

      MessageHandler messageHandler;

      /**
       * @brief q_ptr See Q_DECLARE_PUBLIC
       */
      Zera::XMLConfig::cReader *q_ptr;

      Q_DECLARE_PUBLIC(cReader)
    };
  }
}
#endif // XMLCONFIGREADER_PRIVATE_H

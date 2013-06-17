#include "xmlconfigreader.h"
#include "xmlconfigreader_private.h"

namespace Zera
{
  namespace XMLConfig
  {
    XMLConfigReader::XMLConfigReader(QObject *parent) :
      QObject(parent), d_ptr(new _XMLConfigReaderPrivate(this))
    {
      connect(d_ptr,SIGNAL(finishedParsingXML()), this, SIGNAL(finishedParsingXML()));
      connect(d_ptr,SIGNAL(valueChanged(QString)), this, SIGNAL(valueChanged(QString)));
    }

    bool XMLConfigReader::loadSchema(const QString &filePath)
    {
      return d_ptr->loadSchema(filePath);
    }

    bool XMLConfigReader::loadXML(const QString &filePath)
    {
      return d_ptr->loadXML(filePath);
    }

    QVariant XMLConfigReader::getValue(const QString &key)
    {
      return d_ptr->getValue(key);
    }

    bool XMLConfigReader::setValue(const QString &key, QVariant value)
    {
      return d_ptr->setValue(key, value);
    }

    QString XMLConfigReader::getXMLConfig()
    {
      return d_ptr->getXMLConfig();
    }
  }
}

#ifndef XMLSETTINGS_H
#define XMLSETTINGS_H

#include "xmlconfigreader.h"
#include <QString>
#include <QMap>

/**
  @brief
  XMLSettings is pure virtual class, all derived classes will xml configuration facilitiy
 */
class ZERA_XMLCONFIG_EXPORT XMLSettings: public QObject
{
    Q_OBJECT
public:
    XMLSettings();
public slots:
    virtual void configXMLInfo(QString key) = 0;
protected:
    QMap<QString, quint16> m_ConfigXMLMap;
    Zera::XMLConfig::cReader* m_pXMLReader;
};

#endif // XMLSETTINGS_H

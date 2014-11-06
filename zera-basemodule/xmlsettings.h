#ifndef XMLSETTINGS_H
#define XMLSETTINGS_H

#include <QObject>
#include <QMap>
#include <QString>


namespace Zera
{
namespace XMLConfig
{
    class cReader;
}
}


/**
  @brief
  cXMLSettings is pure virtual class, all derived classes will have xml configuration facilitiy
 */

class cXMLSettings: public QObject
{
    Q_OBJECT

public slots:
    virtual void configXMLInfo(QString key) = 0;

protected:
    QMap<QString,quint16> m_ConfigXMLMap;
    Zera::XMLConfig::cReader* m_pXMLReader;
};

#endif // XMLSETTINGS_H

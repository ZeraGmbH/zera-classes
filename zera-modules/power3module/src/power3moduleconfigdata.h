#ifndef POWER3MODULECONFIGDATA_H
#define POWER3MODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"
#include "foutinfo.h"

namespace POWER3MODULE
{

struct powersystemconfiguration
{
    QString m_sInputU; // entity name for voltage input
    QString m_sInputI; // entity name for current input
};


class cPower3ModuleConfigData
{
public:
    cPower3ModuleConfigData(){}

    quint8 m_nDebugLevel;
    quint8 m_nPowerSystemCount;
    QString m_sInputModule;
    QList<powersystemconfiguration> m_powerSystemConfigList;
};

}
#endif // POWER3MODULECONFIGDATA_H

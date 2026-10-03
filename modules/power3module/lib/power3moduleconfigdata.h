#ifndef POWER3MODULECONFIGDATA_H
#define POWER3MODULECONFIGDATA_H

#include <QStringList>
#include <QList>

namespace POWER3MODULE
{

struct powersystemconfiguration
{
    QString m_sInputU; // component name for voltage input
    QString m_sInputI; // component name for current input
};


class cPower3ModuleConfigData
{
public:
    quint8 m_nPowerSystemCount = 0;
    int m_nModuleId = 0;
    QList<powersystemconfiguration> m_powerSystemConfigList;
    QStringList m_powerChannelList;
};

}
#endif // POWER3MODULECONFIGDATA_H

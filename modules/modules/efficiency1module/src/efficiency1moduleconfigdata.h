#ifndef EFFICIENCY1MODULECONFIGDATA_H
#define EFFICIENCY1MODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"
#include "foutinfo.h"

namespace EFFICIENCY1MODULE
{

struct powerinputconfiguration
{
    int m_nModuleId;
    quint8 m_nPowerSystemCount;
    QStringList powerInputList; // list of used power input components
};


class cEfficiency1ModuleConfigData
{
public:
    cEfficiency1ModuleConfigData(){}

    quint8 m_nDebugLevel;
    powerinputconfiguration m_PowerInputConfiguration;
    powerinputconfiguration m_PowerOutputConfiguration;
};

}
#endif // EFFICIENCY1MODULECONFIGDATA_H

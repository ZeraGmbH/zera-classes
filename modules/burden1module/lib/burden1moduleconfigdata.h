#ifndef BURDEN1MODULECONFIGDATA_H
#define BURDEN1MODULECONFIGDATA_H

#include "configtypes.h"
#include <QStringList>
#include <QList>

namespace BURDEN1MODULE
{

struct burdensystemconfiguration
{
    QString m_sInputVoltageVector; // component name for voltage vector
    QString m_sInputCurrentVector; // component name for current vector
};


class cBurden1ModuleConfigData
{
public:
    cBurden1ModuleConfigData(){}

    quint8 m_nBurdenSystemCount = 0;
    int m_nModuleId = 0;
    QList<burdensystemconfiguration> m_BurdenSystemConfigList;
    QString m_Unit; // A or V
    QStringList m_BurdenChannelList;
    doubleParameter nominalRange;
    stringParameter nominalRangeFactor;
    doubleParameter nominalBurden;
    doubleParameter wireLength;
    doubleParameter wireCrosssection;
};

}
#endif // BURDEN1MODULECONFIGDATA_H

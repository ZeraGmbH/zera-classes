#ifndef BURDEN1MODULECONFIGDATA_H
#define BURDEN1MODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"
#include "foutinfo.h"

namespace BURDEN1MODULE
{

struct burdensystemconfiguration
{
    QString m_sInputVoltageVector; // component name for voltage vector
    QString m_sInputCurrentVector; // component name for current vector
};


// used for configuration export
struct doubleParameter
{
    QString m_sKey;
    double m_fValue;
};


struct stringParameter
{
    QString m_sKey;
    QString m_sPar;
};


class cBurden1ModuleConfigData
{
public:
    cBurden1ModuleConfigData(){}

    quint8 m_nDebugLevel;
    quint8 m_nBurdenSystemCount;
    int m_nModuleId;
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

#ifndef SEC1MODULECONFIGDATA_H
#define SEC1MODULECONFIGDATA_H

#include "secconfigdatacommon.h"
#include <QList>

namespace SEC1MODULE
{

class cSec1ModuleConfigData
{
public:
    TSecCommonReferenceConfigs m_refConfigs;
    TSecCommonDutConfigs m_dutConfigs;
    TSecCommonLimitConfigs m_limitConfigs;

    quint8 m_nModeCount = 0;
    stringParameter m_sMode;
    doubleParameter m_fDutConstant;
    stringParameter m_sDutConstantUnit;
    doubleParameter m_fRefConstant;
    boolParameter m_bContinous;
    doubleParameter m_fT0Value;
    doubleParameter m_fT1Value;
    intParameter m_nTarget;
    doubleParameter m_fEnergy;
    intParameter m_nMRate;
    stringParameter m_sResultUnit;
};

}

#endif // SEC1MODULECONFIGDATA_H

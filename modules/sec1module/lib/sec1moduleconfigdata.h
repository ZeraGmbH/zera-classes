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

    stringParameter m_sMode;
    doubleParameter m_fDutConstant;
    stringParameter m_sDutConstantUnit;
    boolParameter m_bContinous;
    intParameter m_nTarget;
    doubleParameter m_fEnergy;
    intParameter m_nMRate;
    stringParameter m_sResultUnit;
};

}

#endif // SEC1MODULECONFIGDATA_H

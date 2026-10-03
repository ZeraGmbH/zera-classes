#ifndef SEC1MODULECONFIGDATA_H
#define SEC1MODULECONFIGDATA_H

#include "configtypes.h"
#include "secconfigdatacommon.h"
#include <QList>

namespace SEC1MODULE
{

class cSec1ModuleConfigData
{
public:
    quint8 m_nRefInpCount = 0;
    quint8 m_nDutInpCount = 0;
    quint8 m_nModeCount = 0;
    stringParameter m_sRefInput;
    stringParameter m_sDutInput;
    QList<TRefInput> m_refInpList;
    QList<QString> m_dutInpList;
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
    doubleParameter m_fUpperLimit;
    doubleParameter m_fLowerLimit;
    stringParameter m_sResultUnit;
};

}

#endif // SEC1MODULECONFIGDATA_H

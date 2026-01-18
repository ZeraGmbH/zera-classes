#ifndef SEC1MODULECONFIGDATA_H
#define SEC1MODULECONFIGDATA_H

#include "secconfigdatacommon.h"
#include <QList>

namespace SEC1MODULE
{

// used for configuration export


struct boolParameter
{
    QString m_sKey;
    quint8 m_nActive; // active or 1,0
};


struct doubleParameter
{
    QString m_sKey;
    double m_fPar;
};


struct intParameter
{
    QString m_sKey;
    quint32 m_nPar;
};


struct stringParameter
{
    QString m_sKey;
    QString m_sPar;
};


class cSec1ModuleConfigData
{
public:
    cSec1ModuleConfigData(){}
    quint8 m_nRefInpCount;
    quint8 m_nDutInpCount;
    quint8 m_nModeCount;
    stringParameter m_sRefInput;
    stringParameter m_sDutInput;
    QList<TRefInput> m_refInpList;
    QList<QString> m_dutInpList;
    stringParameter m_sMode;
    doubleParameter m_fDutConstant;
    stringParameter m_sDutConstantUnit;
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

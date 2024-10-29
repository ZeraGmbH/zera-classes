#ifndef SEM1MODULECONFIGDATA_H
#define SEM1MODULECONFIGDATA_H

#include "secconfigdatacommon.h"
#include "networkconnectioninfo.h"
#include <QList>

namespace SEM1MODULE
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


class cSem1ModuleConfigData
{
public:
    cSem1ModuleConfigData(){}
    QList<TRefInput> m_refInpList;
    quint8 m_nRefInpCount;
    stringParameter m_sRefInput;
    quint8 m_nActiveUnitCount;
    QList<QString> m_ActiveUnitList;
    quint8 m_nReactiveUnitCount;
    QList<QString> m_ReactiveUnitList;
    quint8 m_nApparentUnitCount;
    QList<QString> m_ApparentUnitList;
    intParameter m_nMeasTime; // time in sec. from 0 to 7200
    boolParameter m_bTargeted;
    doubleParameter m_fUpperLimit;
    doubleParameter m_fLowerLimit;
};

}

#endif // SEM1MODULECONFIGDATA_H

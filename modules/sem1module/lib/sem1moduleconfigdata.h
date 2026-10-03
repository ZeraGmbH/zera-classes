#ifndef SEM1MODULECONFIGDATA_H
#define SEM1MODULECONFIGDATA_H

#include "secconfigdatacommon.h"
#include "configtypes.h"
#include <QList>

namespace SEM1MODULE
{

class cSem1ModuleConfigData
{
public:
    QList<TRefInput> m_refInpList;
    quint8 m_nRefInpCount = 0;
    stringParameter m_sRefInput;
    quint8 m_nActiveUnitCount = 0;
    QList<QString> m_ActiveUnitList;
    quint8 m_nReactiveUnitCount = 0;
    QList<QString> m_ReactiveUnitList;
    quint8 m_nApparentUnitCount = 0;
    QList<QString> m_ApparentUnitList;
    intParameter m_nMeasTime; // time in sec. from 0 to 7200
    boolParameter m_bTargeted;
    doubleParameter m_fUpperLimit;
    doubleParameter m_fLowerLimit;
};

}

#endif // SEM1MODULECONFIGDATA_H

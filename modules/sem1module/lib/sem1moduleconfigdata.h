#ifndef SEM1MODULECONFIGDATA_H
#define SEM1MODULECONFIGDATA_H

#include "secconfigdatacommon.h"
#include <QList>

namespace SEM1MODULE
{

class cSem1ModuleConfigData
{
public:
    TSecCommonReferenceConfigs m_refConfigs;
    TSecCommonLimitConfigs m_limitConfigs;
    TSecCommonUnitConfigs m_unitConfigs;

    intParameter m_nMeasTime; // time in sec. from 0 to 7200
    boolParameter m_bTargeted;
};

}

#endif // SEM1MODULECONFIGDATA_H

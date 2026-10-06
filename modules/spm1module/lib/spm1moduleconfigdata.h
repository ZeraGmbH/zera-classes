#ifndef SPM1MODULECONFIGDATA_H
#define SPM1MODULECONFIGDATA_H

#include "secconfigdatacommon.h"
#include <QList>

namespace SPM1MODULE
{

class cSpm1ModuleConfigData
{
public:
    TSecCommonReferenceConfigs m_refConfigs;
    TSecCommonLimitConfigs m_limitConfigs;
    TSecCommonUnitConfigs m_unitConfigs;

    intParameter m_nMeasTime; // time in sec. from 0 to 7200
    boolParameter m_bTargeted;
};

}

#endif // SPM1MODULECONFIGDATA_H

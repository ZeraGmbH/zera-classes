#ifndef SEMSPMMODULECONFIGDATA_H
#define SEMSPMMODULECONFIGDATA_H

#include "secconfigdatacommon.h"

class SemSpmModuleConfigData
{
public:
    TSecCommonReferenceConfigs m_refConfigs;
    TSecCommonLimitConfigs m_limitConfigs;
    TSecCommonUnitConfigs m_unitConfigs;

    intParameter m_nMeasTime;
    boolParameter m_bTargeted;
};

#endif // SEMSPMMODULECONFIGDATA_H

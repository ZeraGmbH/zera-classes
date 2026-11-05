#ifndef SEMSPMMODULECONFIGDATA_H
#define SEMSPMMODULECONFIGDATA_H

#include "secconfigdatacommon.h"

class SemSpmModuleConfigData
{
public:
    TSecCommonReferenceConfigs m_refConfigs;
    TSecCommonLimitConfigs m_limitConfigs;

    intParameter m_nMeasTime;

    static constexpr int StopModeStartStop = 0;
    static constexpr int StopModeTimed = 1;
    intParameter m_stopMode;
};

#endif // SEMSPMMODULECONFIGDATA_H

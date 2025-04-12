#ifndef MEASUREMENTMODULEFACTORYPARAM_H
#define MEASUREMENTMODULEFACTORYPARAM_H

#include "moduleshareddata.h"
#include "modulegroupnumerator.h"
#include <abstracttcpnetworkfactory.h>

struct ModuleFactoryParam
{
    ModuleFactoryParam(int entityId,
                       int moduleNum,
                       QByteArray configXmlData,
                       ModuleSharedDataPtr moduleSharedData);
    ModuleFactoryParam getAdjustedParam(ModuleGroupNumerator* groupNumerator);
    const int m_entityId;
    const int m_moduleNum;
    const QByteArray m_configXmlData;
    const ModuleSharedDataPtr m_moduleSharedData;
};

#endif // MEASUREMENTMODULEFACTORYPARAM_H

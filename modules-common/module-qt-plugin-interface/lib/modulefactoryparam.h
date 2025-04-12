#ifndef MEASUREMENTMODULEFACTORYPARAM_H
#define MEASUREMENTMODULEFACTORYPARAM_H

#include "moduleshareddata.h"
#include "modulegroupnumerator.h"
#include <abstracttcpnetworkfactory.h>

namespace VeinStorage
{
class AbstractEventSystem;
}


struct ModuleFactoryParam
{
    ModuleFactoryParam(int entityId,
                       int moduleNum,
                       QByteArray configXmlData,
                       VeinStorage::AbstractEventSystem* storagesystem,
                       ModuleSharedDataPtr moduleSharedData);
    ModuleFactoryParam getAdjustedParam(ModuleGroupNumerator* groupNumerator);
    const int m_entityId;
    const int m_moduleNum;
    const QByteArray m_configXmlData;
    VeinStorage::AbstractEventSystem* m_storagesystem;
    const ModuleSharedDataPtr m_moduleSharedData;
};

#endif // MEASUREMENTMODULEFACTORYPARAM_H

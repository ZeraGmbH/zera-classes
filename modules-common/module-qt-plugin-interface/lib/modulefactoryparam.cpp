#include "modulefactoryparam.h"

ModuleFactoryParam::ModuleFactoryParam(int entityId,
                                       int moduleNum,
                                       QByteArray configXmlData,
                                       VeinStorage::AbstractEventSystem *storagesystem,
                                       ModuleSharedDataPtr moduleSharedData) :
    m_entityId(entityId),
    m_moduleNum(moduleNum),
    m_configXmlData(configXmlData),
    m_storagesystem(storagesystem),
    m_moduleSharedData(moduleSharedData)
{
}

ModuleFactoryParam ModuleFactoryParam::getAdjustedParam(ModuleGroupNumerator *groupNumerator)
{
    return ModuleFactoryParam(m_entityId,
                              groupNumerator->requestModuleNum(m_moduleNum),
                              m_configXmlData,
                              m_storagesystem,
                              m_moduleSharedData);
}

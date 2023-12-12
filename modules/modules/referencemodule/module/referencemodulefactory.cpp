#include "referencemodulefactory.h"
#include "referencemodule.h"

namespace REFERENCEMODULE
{

ZeraModules::VirtualModule* ReferenceModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new cReferenceModule(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void ReferenceModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> ReferenceModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString ReferenceModuleFactory::getFactoryName() const
{
    return QString(cReferenceModule::BaseModuleName).toLower();
}

}


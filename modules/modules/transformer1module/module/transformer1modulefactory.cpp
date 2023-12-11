#include "transformer1modulefactory.h"
#include "transformer1module.h"

namespace TRANSFORMER1MODULE
{

ZeraModules::VirtualModule* Transformer1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo)
{
    ZeraModules::VirtualModule *module = new cTransformer1Module(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void Transformer1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Transformer1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Transformer1ModuleFactory::getFactoryName() const
{
    return QString(cTransformer1Module::BaseModuleName).toLower();
}

}


#include "sourcemodulefactory.h"
#include "sourcemodule.h"

ZeraModules::VirtualModule* SourceModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new SourceModule(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}


void SourceModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> SourceModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString SourceModuleFactory::getFactoryName() const
{
    return QString(SourceModule::BaseModuleName).toLower();
}


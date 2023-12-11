#include "spm1modulefactory.h"
#include "spm1module.h"

namespace SPM1MODULE
{

ZeraModules::VirtualModule* Spm1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo)
{
    ZeraModules::VirtualModule *module = new cSpm1Module(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void Spm1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Spm1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Spm1ModuleFactory::getFactoryName() const
{
    return QString(cSpm1Module::BaseModuleName).toLower();
}

}


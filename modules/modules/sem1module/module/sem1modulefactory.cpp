#include "sem1modulefactory.h"
#include "sem1module.h"

namespace SEM1MODULE
{

ZeraModules::VirtualModule* Sem1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo)
{
    ZeraModules::VirtualModule *module = new cSem1Module(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void Sem1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Sem1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Sem1ModuleFactory::getFactoryName() const
{
    return QString(cSem1Module::BaseModuleName).toLower();
}

}


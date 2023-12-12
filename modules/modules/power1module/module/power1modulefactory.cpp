#include "power1modulefactory.h"
#include "power1module.h"

namespace POWER1MODULE
{

ZeraModules::VirtualModule* Power1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new cPower1Module(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void Power1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Power1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Power1ModuleFactory::getFactoryName() const
{
    return QString(cPower1Module::BaseModuleName).toLower();
}

}


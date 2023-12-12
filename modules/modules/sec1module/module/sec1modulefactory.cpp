#include "sec1modulefactory.h"
#include "sec1module.h"

namespace SEC1MODULE
{

ZeraModules::VirtualModule* Sec1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new cSec1Module(moduleNum, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void Sec1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Sec1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Sec1ModuleFactory::getFactoryName() const
{
    return QString(cSec1Module::BaseModuleName).toLower();
}

}


#include "thdnmodulefactory.h"
#include "thdnmodule.h"

namespace THDNMODULE
{

ZeraModules::VirtualModule* ThdnModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo)
{
    ZeraModules::VirtualModule *module = new cThdnModule(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void ThdnModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> ThdnModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString ThdnModuleFactory::getFactoryName() const
{
    return QString(cThdnModule::BaseModuleName).toLower();
}

}


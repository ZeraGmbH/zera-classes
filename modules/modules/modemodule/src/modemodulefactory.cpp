#include "modemodulefactory.h"
#include "modemodule.h"


namespace MODEMODULE
{

ZeraModules::VirtualModule* ModeModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cModeModule(m_ModuleList.count()+1, entityId, storagesystem, demo, parent);
    m_ModuleList.append(module);
    return module;
}


void ModeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> ModeModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString ModeModuleFactory::getFactoryName() const
{
    return QString(cModeModule::BaseModuleName).toLower();
}

}


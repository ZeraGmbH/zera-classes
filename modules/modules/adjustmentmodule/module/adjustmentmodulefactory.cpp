#include "adjustmentmodulefactory.h"
#include "adjustmentmodule.h"

ZeraModules::VirtualModule* AdjustmentModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new cAdjustmentModule(moduleNum, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void AdjustmentModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> AdjustmentModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString AdjustmentModuleFactory::getFactoryName() const
{
    return QString(cAdjustmentModule::BaseModuleName).toLower();
}

#include "adjustmentmodulefactory.h"
#include "adjustmentmodule.h"

namespace ADJUSTMENTMODULE
{

ZeraModules::VirtualModule* AdjustmentModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cAdjustmentModule(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
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
    return QString(BaseModuleName).toLower();
}

}


#include "rangemodulefactory.h"
#include "rangemodule.h"

namespace RANGEMODULE
{

ZeraModules::VirtualModule* RangeModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cRangeModule(m_ModuleList.count()+1, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}


void RangeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> RangeModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString RangeModuleFactory::getFactoryName() const
{
    return QString(cRangeModule::BaseModuleName).toLower();
}

}


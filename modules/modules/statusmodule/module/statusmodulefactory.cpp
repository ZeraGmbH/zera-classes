#include "statusmodulefactory.h"
#include "statusmodule.h"


namespace STATUSMODULE
{

ZeraModules::VirtualModule* StatusModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cStatusModule(m_ModuleList.count()+1, entityId, storagesystem, demo, parent);
    m_ModuleList.append(module);
    return module;
}


void StatusModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> StatusModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString StatusModuleFactory::getFactoryName() const
{
    return QString(cStatusModule::BaseModuleName).toLower();
}

}


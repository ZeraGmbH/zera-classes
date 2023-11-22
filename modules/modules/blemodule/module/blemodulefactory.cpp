#include "blemodulefactory.h"
#include "blemodule.h"

namespace BLEMODULE
{

ZeraModules::VirtualModule* BleModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cBleModule(m_ModuleList.count()+1, entityId, storagesystem, demo, parent);
    m_ModuleList.append(module);
    return module;
}

void BleModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> BleModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString BleModuleFactory::getFactoryName() const
{
    return QString(cBleModule::BaseModuleName).toLower();
}

}

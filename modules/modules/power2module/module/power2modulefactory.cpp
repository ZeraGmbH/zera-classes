#include "power2modulefactory.h"
#include "power2module.h"

namespace POWER2MODULE
{

ZeraModules::VirtualModule* Power2ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new cPower2Module(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void Power2ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Power2ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Power2ModuleFactory::getFactoryName() const
{
    return QString(cPower2Module::BaseModuleName).toLower();
}

}


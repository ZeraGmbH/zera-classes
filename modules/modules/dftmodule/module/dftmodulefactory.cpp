#include "dftmodulefactory.h"
#include "dftmodule.h"

namespace DFTMODULE
{

ZeraModules::VirtualModule* DftModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cDftModule(m_ModuleList.count()+1, entityId, storagesystem, demo, parent);
    m_ModuleList.append(module);
    return module;
}


void DftModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> DftModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString DftModuleFactory::getFactoryName() const
{
    return QString(cDftModule::BaseModuleName).toLower();
}

}


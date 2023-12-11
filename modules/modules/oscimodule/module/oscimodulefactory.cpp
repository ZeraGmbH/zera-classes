#include "oscimodulefactory.h"
#include "oscimodule.h"

namespace OSCIMODULE
{

ZeraModules::VirtualModule* OsciModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo)
{
    ZeraModules::VirtualModule *module = new cOsciModule(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void OsciModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module , &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> OsciModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString OsciModuleFactory::getFactoryName() const
{
    return QString(cOsciModule::BaseModuleName).toLower();
}

}


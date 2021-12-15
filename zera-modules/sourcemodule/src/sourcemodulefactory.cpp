#include "sourcemodulefactory.h"
#include "sourcemodule.h"


namespace SOURCEMODULE
{

ZeraModules::VirtualModule* SourceModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new SourceModule(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}


void SourceModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> SourceModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString SourceModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}


#include "samplemodulefactory.h"
#include "samplemodule.h"

namespace SAMPLEMODULE
{

ZeraModules::VirtualModule* SampleModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem *storagesystem, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cSampleModule(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}

void SampleModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> SampleModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString SampleModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}


#include "lambdamodulefactory.h"
#include "lambdamodule.h"

namespace LAMBDAMODULE
{

ZeraModules::VirtualModule* LambdaModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cLambdaModule(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}

void LambdaModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> LambdaModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString LambdaModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}


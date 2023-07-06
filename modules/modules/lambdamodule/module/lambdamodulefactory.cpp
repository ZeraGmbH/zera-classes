#include "lambdamodulefactory.h"
#include "lambdamodule.h"

namespace LAMBDAMODULE
{

ZeraModules::VirtualModule* LambdaModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cLambdaModule(m_ModuleList.count()+1, entityId, storagesystem, demo, parent);
    m_ModuleList.append(module);
    return module;
}

void LambdaModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> LambdaModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString LambdaModuleFactory::getFactoryName() const
{
    return QString(cLambdaModule::BaseModuleName).toLower();
}

}


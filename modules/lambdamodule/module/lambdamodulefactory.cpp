#include "lambdamodulefactory.h"
#include "lambdamodule.h"

namespace LAMBDAMODULE
{

ZeraModules::VirtualModule* LambdaModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    return new cLambdaModule(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void LambdaModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString LambdaModuleFactory::getFactoryName() const
{
    return QString(cLambdaModule::BaseModuleName).toLower();
}

}


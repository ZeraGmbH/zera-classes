#include "blemodulefactory.h"
#include "blemodule.h"

namespace BLEMODULE
{

ZeraModules::VirtualModule* BleModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cBleModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void BleModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString BleModuleFactory::getFactoryName() const
{
    return QString(cBleModule::BaseModuleName).toLower();
}

}


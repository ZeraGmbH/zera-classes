#include "power2modulefactory.h"
#include "power2module.h"

namespace POWER2MODULE
{

ZeraModules::VirtualModule* Power2ModuleFactory::createModule(MeasurementModuleFactoryParam moduleParam)
{
    return new cPower2Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Power2ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Power2ModuleFactory::getFactoryName() const
{
    return QString(cPower2Module::BaseModuleName).toLower();
}

}


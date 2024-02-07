#include "power3modulefactory.h"
#include "power3module.h"

namespace POWER3MODULE
{

ZeraModules::VirtualModule* Power3ModuleFactory::createModule(MeasurementModuleFactoryParam moduleParam)
{
    return new cPower3Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Power3ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Power3ModuleFactory::getFactoryName() const
{
    return QString(cPower3Module::BaseModuleName).toLower();
}

}


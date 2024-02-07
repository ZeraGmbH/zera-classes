#include "modemodulefactory.h"
#include "modemodule.h"


namespace MODEMODULE
{

ZeraModules::VirtualModule* ModeModuleFactory::createModule(MeasurementModuleFactoryParam moduleParam)
{
    return new cModeModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}


void ModeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString ModeModuleFactory::getFactoryName() const
{
    return QString(cModeModule::BaseModuleName).toLower();
}

}


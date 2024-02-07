#include "statusmodulefactory.h"
#include "statusmodule.h"


namespace STATUSMODULE
{

ZeraModules::VirtualModule* StatusModuleFactory::createModule(MeasurementModuleFactoryParam moduleParam)
{
    return new cStatusModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}


void StatusModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString StatusModuleFactory::getFactoryName() const
{
    return QString(cStatusModule::BaseModuleName).toLower();
}

}


#include "burden1modulefactory.h"
#include "burden1module.h"

namespace BURDEN1MODULE
{

ZeraModules::VirtualModule* Burden1ModuleFactory::createModule(MeasurementModuleFactoryParam moduleParam)
{
    return new cBurden1Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Burden1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Burden1ModuleFactory::getFactoryName() const
{
    return QString(cBurden1Module::BaseModuleName).toLower();
}

}


#include "efficiency1modulefactory.h"
#include "efficiency1module.h"

namespace EFFICIENCY1MODULE
{

ZeraModules::VirtualModule* Efficiency1ModuleFactory::createModule(MeasurementModuleFactoryParam moduleParam)
{
    return new cEfficiency1Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Efficiency1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Efficiency1ModuleFactory::getFactoryName() const
{
    return QString(cEfficiency1Module::BaseModuleName).toLower();
}

}


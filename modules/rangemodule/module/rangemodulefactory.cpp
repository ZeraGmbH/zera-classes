#include "rangemodulefactory.h"
#include "rangemodule.h"

namespace RANGEMODULE
{

ZeraModules::VirtualModule* RangeModuleFactory::createModule(MeasurementModuleFactoryParam moduleParam)
{
    return new cRangeModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}


void RangeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString RangeModuleFactory::getFactoryName() const
{
    return QString(cRangeModule::BaseModuleName).toLower();
}

}


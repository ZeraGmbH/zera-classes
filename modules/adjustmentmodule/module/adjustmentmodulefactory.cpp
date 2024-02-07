#include "adjustmentmodulefactory.h"
#include "adjustmentmodule.h"

ZeraModules::VirtualModule* AdjustmentModuleFactory::createModule(MeasurementModuleFactoryParam moduleParam)
{
    return new cAdjustmentModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void AdjustmentModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}

QString AdjustmentModuleFactory::getFactoryName() const
{
    return QString(cAdjustmentModule::BaseModuleName).toLower();
}

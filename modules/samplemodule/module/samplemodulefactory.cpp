#include "samplemodulefactory.h"
#include "samplemodule.h"

namespace SAMPLEMODULE
{

ZeraModules::VirtualModule* SampleModuleFactory::createModule(MeasurementModuleFactoryParam moduleParam)
{
    return new cSampleModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void SampleModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString SampleModuleFactory::getFactoryName() const
{
    return QString(cSampleModule::BaseModuleName).toLower();
}

}


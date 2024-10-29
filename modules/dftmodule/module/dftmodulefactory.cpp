#include "dftmodulefactory.h"
#include "dftmodule.h"

namespace DFTMODULE
{

ZeraModules::VirtualModule* DftModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cDftModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}


void DftModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString DftModuleFactory::getFactoryName() const
{
    return QString(cDftModule::BaseModuleName).toLower();
}

}


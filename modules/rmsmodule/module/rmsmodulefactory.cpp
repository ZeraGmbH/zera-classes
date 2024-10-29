#include <QDebug>

#include "rmsmodulefactory.h"
#include "rmsmodule.h"

namespace RMSMODULE
{

ZeraModules::VirtualModule* RmsModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cRmsModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}


void RmsModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString RmsModuleFactory::getFactoryName() const
{
    return QString(cRmsModule::BaseModuleName).toLower();
}

}


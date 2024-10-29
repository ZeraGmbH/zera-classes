#include "oscimodulefactory.h"
#include "oscimodule.h"

namespace OSCIMODULE
{

ZeraModules::VirtualModule* OsciModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cOsciModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void OsciModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module , &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString OsciModuleFactory::getFactoryName() const
{
    return QString(cOsciModule::BaseModuleName).toLower();
}

}


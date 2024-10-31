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
    module->startDestroy();
}

QString OsciModuleFactory::getFactoryName() const
{
    return QString(cOsciModule::BaseModuleName).toLower();
}

}


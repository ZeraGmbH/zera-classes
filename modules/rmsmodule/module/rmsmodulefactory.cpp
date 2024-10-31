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
    module->startDestroy();
}

QString RmsModuleFactory::getFactoryName() const
{
    return QString(cRmsModule::BaseModuleName).toLower();
}

}


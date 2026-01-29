#include "blemodulefactory.h"
#include "blemodule.h"

namespace BLEMODULE
{

ZeraModules::VirtualModule* BleModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cBleModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void BleModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString BleModuleFactory::getFactoryName() const
{
    return QString(cBleModule::BaseModuleName).toLower();
}

}


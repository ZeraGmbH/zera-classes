#include "hotplugcontrolsmodulefactory.h"
#include "hotplugcontrolsmodule.h"

namespace HOTPLUGCONTROLSMODULE
{

ZeraModules::VirtualModule* HotplugControlsModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cHotplugControlsModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void HotplugControlsModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString HotplugControlsModuleFactory::getFactoryName() const
{
    return QString(cHotplugControlsModule::BaseModuleName).toLower();
}

}


#include "samplemodulefactory.h"
#include "samplemodule.h"

namespace SAMPLEMODULE
{

ZeraModules::VirtualModule* SampleModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new cSampleModule(moduleNum, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void SampleModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString SampleModuleFactory::getFactoryName() const
{
    return QString(cSampleModule::BaseModuleName).toLower();
}

}


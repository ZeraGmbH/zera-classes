#include "fftmodulefactory.h"
#include "fftmodule.h"

namespace FFTMODULE
{

ZeraModules::VirtualModule* FftModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo, int moduleNum)
{
    return new cFftModule(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void FftModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString FftModuleFactory::getFactoryName() const
{
    return QString(cFftModule::BaseModuleName).toLower();
}

}


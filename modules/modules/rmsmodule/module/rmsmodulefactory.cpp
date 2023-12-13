#include <QDebug>

#include "rmsmodulefactory.h"
#include "rmsmodule.h"

namespace RMSMODULE
{

ZeraModules::VirtualModule* RmsModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    return new cRmsModule(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
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


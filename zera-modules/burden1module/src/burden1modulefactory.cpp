#include "burden1modulefactory.h"
#include "burden1module.h"

namespace BURDEN1MODULE
{

ZeraModules::VirtualModule* Burden1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cBurden1Module(m_ModuleList.count()+1, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}

void Burden1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Burden1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Burden1ModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}


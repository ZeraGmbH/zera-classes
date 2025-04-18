#include "taskmoduledeactivate.h"

namespace ZeraModules
{

TaskTemplatePtr TaskModuleDeactivate::create(VirtualModule *module, AbstractModuleFactory *moduleFactory)
{
    return std::make_unique<TaskModuleDeactivate>(module, moduleFactory);
}

TaskModuleDeactivate::TaskModuleDeactivate(VirtualModule *module, AbstractModuleFactory *moduleFactory) :
    m_module(module),
    m_moduleFactory(moduleFactory)
{
}

void TaskModuleDeactivate::start()
{
    qInfo("Start deactivate %s / %i...", qPrintable(m_module->getVeinModuleName()), m_module->getEntityId());
    connect(m_module, &ZeraModules::VirtualModule::moduleDeactivated, this, [this]() {
        qInfo("%s / %i deactivated.", qPrintable(m_module->getVeinModuleName()), m_module->getEntityId());
        finishTask(true);
    });
    m_moduleFactory->destroyModule(m_module);
}

}

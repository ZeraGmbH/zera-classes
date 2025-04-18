#include "taskmoduledelayeddelete.h"

namespace ZeraModules
{

TaskTemplatePtr TaskModuleDelayedDelete::create(VirtualModule *module)
{
    return std::make_unique<TaskModuleDelayedDelete>(module);
}

TaskModuleDelayedDelete::TaskModuleDelayedDelete(VirtualModule *module) :
    m_module(module),
    m_entityId(m_module->getEntityId()),
    m_veinModuleName(m_module->getVeinModuleName())
{
}

void TaskModuleDelayedDelete::start()
{
    qInfo("Start delete %s / %i...", qPrintable(m_veinModuleName), m_entityId);
    connect(m_module, &QObject::destroyed, this, [this]() {
        qInfo("%s / %i deleted.", qPrintable(m_veinModuleName), m_entityId);
        finishTask(true);
    });
    m_module->deleteLater();
}

}

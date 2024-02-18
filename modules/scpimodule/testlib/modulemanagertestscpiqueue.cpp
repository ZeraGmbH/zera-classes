#include "modulemanagertestscpiqueue.h"
#include <timemachineobject.h>
#include <QFile>
#include <QFileInfo>

ModuleManagerTestScpiQueue::ModuleManagerTestScpiQueue()
{
    addSystem(&m_storageSystem);
}

void ModuleManagerTestScpiQueue::addModule(cBaseModule *module)
{
    connect(module, &ZeraModules::VirtualModule::addEventSystem, this, [&] (VeinEvent::EventSystem *eventSystem) {
        addSystem(eventSystem);
    });

    TimeMachineObject::feedEventLoop();
    module->startModule();
    TimeMachineObject::feedEventLoop();
}

VeinStorage::VeinHash *ModuleManagerTestScpiQueue::getStorageSystem()
{
    return &m_storageSystem;
}

void ModuleManagerTestScpiQueue::addSystem(VeinEvent::EventSystem *subsystem)
{
    if(!m_addedSubsystems.contains(subsystem)) {
        // On startup modules can emit addEventSystem multiple times for some
        // systems. So add them once only (see ModuleEventHandler::addSystem -
        // it does same)
        m_addedSubsystems.insert(subsystem);
        m_eventHandler.addSubsystem(subsystem);
    }
}

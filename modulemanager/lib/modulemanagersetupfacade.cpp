#include "modulemanagersetupfacade.h"

ModuleManagerSetupFacade::ModuleManagerSetupFacade(QObject *parent, bool devMode) :
    m_eventHandler(this),
    m_mmController(this, devMode),
    m_introspectionSystem(this),
    m_storSystem(this)
{
    m_eventHandler.addSubsystem(&m_mmController);
    m_eventHandler.addSubsystem(&m_introspectionSystem);
    m_eventHandler.addSubsystem(&m_storSystem);
    m_mmController.setStorage(&m_storSystem);
}

void ModuleManagerSetupFacade::addSubsystem(VeinEvent::EventSystem *subsystem)
{
    m_eventHandler.addSubsystem(subsystem);
}

void ModuleManagerSetupFacade::addSystem(VeinEvent::EventSystem *system)
{
    m_eventHandler.addSystem(system);
}

void ModuleManagerSetupFacade::clearSystems()
{
    m_eventHandler.clearSystems();
}

ModuleManagerController *ModuleManagerSetupFacade::getModuleManagerController()
{
    return &m_mmController;
}

VeinStorage::VeinHash *ModuleManagerSetupFacade::getStorageSystem()
{
    return &m_storSystem;
}

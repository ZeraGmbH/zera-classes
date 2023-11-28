#include "modulemanagersetupfacade.h"

ModuleManagerSetupFacade::ModuleManagerSetupFacade(bool demo) :
    m_mmController(this, demo)
{
    m_eventHandler.addSubsystem(&m_mmController);
    m_eventHandler.addSubsystem(&m_introspectionSystem);
}

void ModuleManagerSetupFacade::addSubsystem(VeinEvent::EventSystem *subsystem)
{
    m_eventHandler.addSubsystem(subsystem);
}

void ModuleManagerSetupFacade::addModuleSystem(VeinEvent::EventSystem *system)
{
    m_eventHandler.addSystem(system);
}

void ModuleManagerSetupFacade::clearEventHandlerSystems()
{
    m_eventHandler.clearSystems();
}

ModuleManagerController *ModuleManagerSetupFacade::getModuleManagerController()
{
    return &m_mmController;
}

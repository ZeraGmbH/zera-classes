#include "modulemanagersetupfacade.h"

ModuleManagerSetupFacade::ModuleManagerSetupFacade()
{

}

void ModuleManagerSetupFacade::setSubsystems(QList<VeinEvent::EventSystem *> subsystems)
{
    m_eventHandler.setSubsystems(subsystems);
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

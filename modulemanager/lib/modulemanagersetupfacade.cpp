#include "modulemanagersetupfacade.h"

ModuleManagerSetupFacade::ModuleManagerSetupFacade(QObject *parent, bool devMode) :
    m_eventHandler(this),
    m_mmController(this, devMode)
{
    m_eventHandler.addSubsystem(&m_mmController);
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

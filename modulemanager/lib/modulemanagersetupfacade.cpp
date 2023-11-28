#include "modulemanagersetupfacade.h"

ModuleManagerSetupFacade::ModuleManagerSetupFacade(LicenseSystemInterface *licenseSystem, bool demo) :
    m_mmController(this, demo),
    m_licenseSystem(licenseSystem)
{
    m_eventHandler.addSubsystem(&m_mmController);
    m_eventHandler.addSubsystem(&m_introspectionSystem);
    m_eventHandler.addSubsystem(&m_storSystem);
    m_eventHandler.addSubsystem(m_licenseSystem);
    m_mmController.setStorage(&m_storSystem);
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

VeinStorage::VeinHash *ModuleManagerSetupFacade::getStorageSystem()
{
    return &m_storSystem;
}

LicenseSystemInterface *ModuleManagerSetupFacade::getLicenseSystem()
{
    return m_licenseSystem;
}

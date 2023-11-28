#ifndef MODULEMANAGERSETUPFACADE_H
#define MODULEMANAGERSETUPFACADE_H

#include "moduleeventhandler.h"

class ModuleManagerSetupFacade
{
public:
    ModuleManagerSetupFacade();
    void setSubsystems(QList<VeinEvent::EventSystem*> subsystems);
    void addSubsystem(VeinEvent::EventSystem* subsystem);
    void addModuleSystem(VeinEvent::EventSystem* system);
    void clearEventHandlerSystems();
private:
    ModuleEventHandler m_eventHandler;
};

#endif // MODULEMANAGERSETUPFACADE_H

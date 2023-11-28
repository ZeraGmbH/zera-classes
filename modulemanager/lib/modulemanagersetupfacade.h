#ifndef MODULEMANAGERSETUPFACADE_H
#define MODULEMANAGERSETUPFACADE_H

#include "moduleeventhandler.h"
#include "modulemanagercontroller.h"

class ModuleManagerSetupFacade : public QObject
{
    Q_OBJECT
public:
    ModuleManagerSetupFacade(bool demo = false);
    void addSubsystem(VeinEvent::EventSystem* subsystem);
    void addModuleSystem(VeinEvent::EventSystem* system);
    void clearEventHandlerSystems();
    ModuleManagerController *getModuleManagerController();
private:
    ModuleEventHandler m_eventHandler;
    ModuleManagerController m_mmController;
};

#endif // MODULEMANAGERSETUPFACADE_H

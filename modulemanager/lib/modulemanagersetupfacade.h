#ifndef MODULEMANAGERSETUPFACADE_H
#define MODULEMANAGERSETUPFACADE_H

#include "moduleeventhandler.h"
#include "modulemanagercontroller.h"

class ModuleManagerSetupFacade : public QObject
{
    Q_OBJECT
public:
    ModuleManagerSetupFacade(QObject *parent = nullptr, bool devMode = false);
    void addSubsystem(VeinEvent::EventSystem* subsystem);
    void addSystem(VeinEvent::EventSystem* system);
    void clearSystems();
    ModuleManagerController *getModuleManagerController();
private:
    ModuleEventHandler m_eventHandler;
    ModuleManagerController m_mmController;
};

#endif // MODULEMANAGERSETUPFACADE_H

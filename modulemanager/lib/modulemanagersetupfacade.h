#ifndef MODULEMANAGERSETUPFACADE_H
#define MODULEMANAGERSETUPFACADE_H

#include "moduleeventhandler.h"
#include "modulemanagercontroller.h"
#include "vn_introspectionsystem.h"

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
    VeinNet::IntrospectionSystem m_introspectionSystem;
};

#endif // MODULEMANAGERSETUPFACADE_H

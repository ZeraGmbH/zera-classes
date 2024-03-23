#ifndef MODULEMANAGERSETUPFACADE_H
#define MODULEMANAGERSETUPFACADE_H

#include "licensesysteminterface.h"
#include "moduleeventhandler.h"
#include "modulemanagercontroller.h"
#include "vn_introspectionsystem.h"
#include "vs_veinhash.h"

class ModuleManagerSetupFacade : public QObject
{
    Q_OBJECT
public:
    ModuleManagerSetupFacade(LicenseSystemInterface *licenseSystem, bool devMode = false, QObject *parent = nullptr);
    static void registerMetaTypeStreamOperators();
    void addSubsystem(VeinEvent::EventSystem* subsystem);
    void addSystem(VeinEvent::EventSystem* system);
    void clearSystems();
    ModuleManagerController *getModuleManagerController();
    VeinEvent::StorageSystem *getStorageSystem();
    LicenseSystemInterface *getLicenseSystem();
private:
    ModuleEventHandler m_eventHandler;
    VeinStorage::VeinHash m_storSystem;
    ModuleManagerController m_mmController;
    VeinNet::IntrospectionSystem m_introspectionSystem;
    LicenseSystemInterface *m_licenseSystem = nullptr;
};

#endif // MODULEMANAGERSETUPFACADE_H

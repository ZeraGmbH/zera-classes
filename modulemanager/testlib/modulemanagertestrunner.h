#ifndef MODULEMANAGERTESTRUNNER_H
#define MODULEMANAGERTESTRUNNER_H

#include "testlicensesystem.h"
#include "modulemanagersetupfacade.h"
#include "testdspinterface.h"
#include "testfactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include "vf_cmd_event_handler_system.h"
#include <QObject>
#include <memory>

class ModuleManagerTestRunner : public QObject
{
    Q_OBJECT
public:
    ModuleManagerTestRunner(QString sessionFileName, bool initialAdjPermission = false, QString deviceName = "mt310s2");
    ~ModuleManagerTestRunner();
    VeinEvent::StorageSystem *getVeinStorageSystem();
    const QList<TestDspInterfacePtr>& getDspInterfaceList() const;
    VfCmdEventHandlerSystemPtr getVfCmdEventHandlerSystemPtr();
    ZeraModules::VirtualModule *getModule(QString uniqueName, int entityId);
    void setVfComponent(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem, int moduleId, QString componentName, QVariant oldValue, QVariant newValue);
private:
    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
    TestFactoryServiceInterfacesPtr m_serviceInterfaceFactory;

    VfCmdEventHandlerSystemPtr m_vfCmdEventHandlerSystem;
};

#endif // MODULEMANAGERTESTRUNNER_H

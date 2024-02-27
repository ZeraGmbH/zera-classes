#ifndef MODULEMANAGERTESTRUNNER_H
#define MODULEMANAGERTESTRUNNER_H

#include "licensesystemmock.h"
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
    ModuleManagerTestRunner(QString sessionFileName, QString deviceName = "mt310s2");
    ~ModuleManagerTestRunner();
    VeinStorage::VeinHash* getVeinStorageSystem();
    const QList<TestDspInterfacePtr>& getDspInterfaceList() const;
    VfCmdEventHandlerSystemPtr getVfCmdEventHandlerSystemPtr();
private:
    std::unique_ptr<LicenseSystemMock> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
    TestFactoryServiceInterfacesPtr m_serviceInterfaceFactory;

    VfCmdEventHandlerSystemPtr m_vfCmdEventHandlerSystem;
};

#endif // MODULEMANAGERTESTRUNNER_H

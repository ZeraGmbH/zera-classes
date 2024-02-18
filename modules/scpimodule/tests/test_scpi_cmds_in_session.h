#ifndef TEST_SCPI_CMDS_IN_SESSION_H
#define TEST_SCPI_CMDS_IN_SESSION_H

#include "licensesystemmock.h"
#include "modulemanagersetupfacade.h"
#include "testmodulemanager.h"
#include <QObject>
#include <memory>

class test_scpi_cmds_in_session : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void initialSession();
    void initialTestClient();
    void minScpiDevIface();
    void initialScpiCommandsOnOtherModules();
    void multiReadDoubleDeleteCrasher();
    void devIfaceVeinComponent();
    void devIfaceVeinComponentMultipleEntities();
    void devIfaceVeinComponentMultipleEntitiesForLongXml();

private:
    void setupServices(QString sessionFileName);

    std::unique_ptr<LicenseSystemMock> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
};

#endif // TEST_SCPI_CMDS_IN_SESSION_H

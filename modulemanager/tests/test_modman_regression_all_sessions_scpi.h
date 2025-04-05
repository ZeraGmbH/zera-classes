#ifndef TEST_MODMAN_REGRESSION_ALL_SESSIONS_SCPI_H
#define TEST_MODMAN_REGRESSION_ALL_SESSIONS_SCPI_H

#include "abstractfactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include <scpimodulefortest.h>
#include <scpitestclient.h>
#include <modulemanagersetupfacade.h>
#include <testlicensesystem.h>

class test_modman_regression_all_sessions_scpi : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void initTestCase_data();
    void init();
    void cleanup();

    void testScpiMeasure();
private:
    QByteArray deduceDevice(const QString &sessionFileName);

    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
    SCPIMODULE::ScpiModuleForTest *m_scpiModule = nullptr;
    SCPIMODULE::ScpiTestClient *m_scpiclient = nullptr;
    QString m_scpiResponse;
    struct TestRow {
        QByteArray testName;
        QString deviceName;
        QString sessionFile;
    };
    QList<TestRow> m_testRowData;
};

#endif // TEST_MODMAN_REGRESSION_ALL_SESSIONS_SCPI_H

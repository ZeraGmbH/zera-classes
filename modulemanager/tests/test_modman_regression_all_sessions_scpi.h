#ifndef TEST_MODMAN_REGRESSION_ALL_SESSIONS_SCPI_H
#define TEST_MODMAN_REGRESSION_ALL_SESSIONS_SCPI_H

#include "abstractfactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include <scpimodule.h>
#include <scpitestclient.h>
#include <modulemanagersetupfacade.h>
#include <testlicensesystem.h>

class test_modman_regression_all_sessions_scpi : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void testScpi_data();
    void testScpi();

private:
    QStringList getAvailableSessions();
    static QString deduceDevice(const QString &sessionFileName);
    static QString formatForDump(const QString &scpiResponse);
    static QString genFilePath(QString sessionFile, const QString &scpiCmd);

    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
    SCPIMODULE::cSCPIModule *m_scpiModule = nullptr;
    SCPIMODULE::ScpiTestClient *m_scpiclient = nullptr;
    QStringList m_scpiResponses;
    QString m_lastSessionFile;
};

#endif // TEST_MODMAN_REGRESSION_ALL_SESSIONS_SCPI_H

#ifndef TEST_SCPI_PENETRATE_H
#define TEST_SCPI_PENETRATE_H

#include <QObject>
#include "testmodulemanager.h"
#include <scpimodulefortest.h>
#include <scpitestclient.h>
#include <modulemanagersetupfacade.h>
#include <testlicensesystem.h>

class test_scpi_penetrate : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void penetrateRangeSet_data();
    void penetrateRangeSet();

private:
    QStringList getAvailableSessions();
    void createScpiClient();
    static QString deduceDevice(const QString &sessionFileName);
    static QString formatForDump(const QString &scpiResponse);
    static QString genFilePath(QString sessionFile, const QString &scpiCmd);

    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
    SCPIMODULE::ScpiModuleForTest *m_scpiModule = nullptr;
    SCPIMODULE::ScpiTestClient *m_scpiclient = nullptr;
    QStringList m_scpiResponses;
    QString m_lastSessionFile;
};

#endif // TEST_SCPI_PENETRATE_H

#include "test_change_session.h"
#include "testfactoryserviceinterfaces.h"
#include "modulemanagerconfig.h"
#include "scpimoduleclientblocked.h"
#include "testmodulemanager.h"
#include "vf_client_component_setter.h"
#include <modulemanagersetupfacade.h>
#include <testlicensesystem.h>
#include <zera-jsonfileloader.h>
#include <timemachineobject.h>
#include <memoryalloctracker.h>
#include <backtracetreegenerator.h>
#include <QJsonArray>
#include <QTest>


QTEST_MAIN(test_change_session)

static int constexpr systemEntityId = 0;

void test_change_session::initTestCase()
{
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_change_session::changeToUnavailableSession()
{
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QVariant oldValue = modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "XYZ");
    emit modManSetupFacade.getStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-meas-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeToSameSession()
{
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QVariant oldValue = modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "mt310s2-meas-session.json");
    emit modManSetupFacade.getStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-meas-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeSessionMt310s2FromComponent()
{
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QVariant oldValue = modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "mt310s2-dc-session.json");
    emit modManSetupFacade.getStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-dc-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeSessionMt310s2SCPICmd()
{
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    ScpiModuleClientBlocked client;
    client.sendReceive("CONFIGURATION:SYST:NAMESESSION EMOB DC;");
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-emob-session-dc.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeSessionCom5003FromComponent()
{
    ModulemanagerConfig::setDemoDevice("com5003");
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("com5003", false);
    modMan.changeSessionFile("com5003-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QVariant oldValue = modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "com5003-ced-session.json");
    emit modManSetupFacade.getStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session").toString(), QString("com5003-ced-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeSessionCom5003SCPICmd()
{
    ModulemanagerConfig::setDemoDevice("com5003");
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("com5003", false);
    modMan.changeSessionFile("com5003-meas-session.json");
    modMan.waitUntilModulesAreReady();

    ScpiModuleClientBlocked client;
    client.sendReceive("CONFIGURATION:SYST:NAMESESSION 3 Systems / 2 Wires;");
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modManSetupFacade.getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session").toString(), QString("com5003-perphase-session.json"));
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::changeSessionMt310s2MultipleProblematicRangeModule()
{
    QSKIP("Mem test takes ages...");

    // control object's lifetime to play around
    std::unique_ptr<TestLicenseSystem> licenseSystem = std::make_unique<TestLicenseSystem>();
    std::unique_ptr<ModuleManagerSetupFacade> modManSetupFacade = std::make_unique<ModuleManagerSetupFacade>(licenseSystem.get());

    std::unique_ptr<TestModuleManager> modMan = std::make_unique<TestModuleManager>(modManSetupFacade.get(), m_serviceInterfaceFactory);
    modMan->loadAllAvailableModulePlugins();
    modMan->setupConnections();
    modMan->startAllTestServices("mt310s2", false);

    MemoryAllocTracker memAllocTracker;
    memAllocTracker.start();

    constexpr int countSessionChange = 10;
    for (int sessionNo=0; sessionNo<countSessionChange; sessionNo++) {
        const QString sessionFileName = QString(":/sessions/session-problematic-range-%1.json").arg(sessionNo%2);
        qInfo("%i: Session: %s", sessionNo, qPrintable(sessionFileName));
        modMan->changeSessionFile(sessionFileName);
        modMan->waitUntilModulesAreReady();
    }

    modMan->destroyModulesAndWaitUntilAllShutdown();
    modMan.reset();
    modManSetupFacade.reset();
    licenseSystem.reset();

    QTest::qWait(10000); // wait for QArrayData::allocate relax (QAbstractSocket)
    memAllocTracker.stop();
    BacktraceTreeGenerator allocTree(memAllocTracker.getAllocationsTimeSorted());
    qInfo("Unfreed %i", allocTree.getEntryList()->count());
}

void test_change_session::changeSessionMt310s2MultipleProblematicSampleModule()
{
    QSKIP("This takes ages and is just there to isolate mem-eaters on multiple session change");

    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);

    constexpr int countSessionChange = 1000;
    for (int sessionNo=0; sessionNo<countSessionChange; sessionNo++) {
        const QString sessionFileName = QString(":/sessions/session-problematic-sample-%1.json").arg(sessionNo%2);
        qInfo("%i: Session: %s", sessionNo, qPrintable(sessionFileName));
        modMan.changeSessionFile(sessionFileName);
        modMan.waitUntilModulesAreReady();
    }
    modMan.destroyModulesAndWaitUntilAllShutdown();
}

void test_change_session::testSessionCatalogScpiCmd()
{
    ModulemanagerConfig::setDemoDevice("mt310s2");
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    ScpiModuleClientBlocked client;
    QString receivedSessionList = client.sendReceive("CONFIGURATION:SYST:SESSION:CATALOG?");
    QJsonObject jsonConfig = cJsonFileLoader::loadJsonFile(ModulemanagerConfig::getConfigFileNameFull()).value("mt310s2").toObject();
    QJsonArray jsonSessionArray = jsonConfig.value("sessionDisplayStrings").toArray();
    QString expectedSessionList = jsonSessionArray.at(0).toString();
    for (int i = 1; i < jsonSessionArray.count(); i++) {
        expectedSessionList = expectedSessionList + ";" + jsonSessionArray.at(i).toString();
    }
    QCOMPARE(receivedSessionList, expectedSessionList);

    modMan.destroyModulesAndWaitUntilAllShutdown();
}

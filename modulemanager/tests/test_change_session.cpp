#include "test_change_session.h"
#include "modulemanagertestrunner.h"
#include "modulemanagerconfig.h"
#include "scpimodulenetclientblocked.h"
#include "vf_client_component_setter.h"
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <zera-jsonfileloader.h>
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <memoryalloctracker.h>
#include <backtracetreegenerator.h>
#include <QJsonArray>
#include <QTest>

QTEST_MAIN(test_change_session)

static int constexpr systemEntityId = 0;

void test_change_session::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
}

void test_change_session::changeToUnavailableSession()
{
    ModuleManagerTestRunner modMan("mt310s2-meas-session.json");

    QVariant oldValue = modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "XYZ");
    emit modMan.getVeinStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-meas-session.json"));
}

void test_change_session::changeToSameSession()
{
    ModuleManagerTestRunner modMan("mt310s2-meas-session.json");

    QVariant oldValue = modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "mt310s2-meas-session.json");
    emit modMan.getVeinStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-meas-session.json"));
}

void test_change_session::changeSessionMt310s2FromComponent()
{
    ModuleManagerTestRunner modMan("mt310s2-meas-session.json");

    QVariant oldValue = modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "mt310s2-dc-session.json");
    emit modMan.getVeinStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-dc-session.json"));
}

void test_change_session::changeSessionMt310s2SCPICmd()
{
    ModuleManagerTestRunner modMan("mt310s2-meas-session.json");

    ScpiModuleNetClientBlocked client;
    client.sendReceive("CONFIGURATION:SYST:NAMESESSION EMOB DC;");
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session").toString(), QString("mt310s2-emob-session-dc.json"));
}

void test_change_session::changeSessionCom5003FromComponent()
{
    ModulemanagerConfig::setDemoDevice("com5003");
    ModuleManagerTestRunner modMan("com5003-meas-session.json", false, "com5003");

    QVariant oldValue = modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session");
    QEvent* event = VfClientComponentSetter::generateEvent(systemEntityId, "Session", oldValue, "com5003-ced-session.json");
    emit modMan.getVeinStorageSystem()->sigSendEvent(event); // could be any event system
    modMan.waitUntilModulesAreReady();
    QCOMPARE(modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session").toString(), QString("com5003-ced-session.json"));
}

void test_change_session::changeSessionCom5003SCPICmd()
{
    ModulemanagerConfig::setDemoDevice("com5003");
    ModuleManagerTestRunner modMan("com5003-meas-session.json", false, "com5003");

    VeinStorage::AbstractDatabase *storageDb = modMan.getVeinStorageDb();
    QByteArray veinDumpBefore = VeinStorage::DumpJson::dumpToByteArray(storageDb, QList<int>(), QList<int>(), true);
    bool allOk = true;
    if(!TestLogHelpers::compareAndLogOnDiffJsonFile(":/veinDumps/com5003-meas-session-no-values.json", veinDumpBefore))
        allOk = false;

    ScpiModuleNetClientBlocked client;
    client.sendReceive("CONFIGURATION:SYST:NAMESESSION 3 Systems / 2 Wires;");
    modMan.waitUntilModulesAreReady();

    QByteArray veinDumpAfter = VeinStorage::DumpJson::dumpToByteArray(storageDb, QList<int>(), QList<int>(), true);
    if(!TestLogHelpers::compareAndLogOnDiffJsonFile(":/veinDumps/com5003-perphase-session-no-values.json", veinDumpAfter))
        allOk = false;
    QVERIFY(allOk);
    QCOMPARE(modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session").toString(), QString("com5003-perphase-session.json"));
}

void test_change_session::changeSessionCom5003SCPICmdRefSession()
{
    ModulemanagerConfig::setDemoDevice("com5003");
    ModuleManagerTestRunner modMan("com5003-meas-session.json", false, "com5003");

    VeinStorage::AbstractDatabase *storageDb = modMan.getVeinStorageDb();
    QByteArray veinDumpBefore = VeinStorage::DumpJson::dumpToByteArray(storageDb, QList<int>(), QList<int>(), true);
    bool allOk = true;
    if(!TestLogHelpers::compareAndLogOnDiffJsonFile(":/veinDumps/com5003-meas-session-no-values.json", veinDumpBefore))
        allOk = false;

    // reference session is special due to change of channel names
    ScpiModuleNetClientBlocked client;
    client.sendReceive("CONFIGURATION:SYST:NAMESESSION Reference;");
    modMan.waitUntilModulesAreReady();

    QByteArray veinDumpAfter = VeinStorage::DumpJson::dumpToByteArray(storageDb, QList<int>(), QList<int>(), true);
    if(!TestLogHelpers::compareAndLogOnDiffJsonFile(":/veinDumps/com5003-ref-session-no-values.json", veinDumpAfter))
        allOk = false;
    QVERIFY(allOk);
    QCOMPARE(modMan.getVeinStorageDb()->getStoredValue(systemEntityId, "Session").toString(), QString("com5003-ref-session.json"));
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
    ModuleManagerTestRunner modMan("mt310s2-meas-session.json");

    ScpiModuleNetClientBlocked client;
    QString receivedSessionList = client.sendReceive("CONFIGURATION:SYST:SESSION:CATALOG?");
    QJsonObject jsonConfig = cJsonFileLoader::loadJsonFile(ModulemanagerConfig::getConfigFileNameFull()).value("mt310s2").toObject();
    QJsonArray jsonSessionArray = jsonConfig.value("sessionDisplayStrings").toArray();
    QString expectedSessionList = jsonSessionArray.at(0).toString();
    for (int i = 1; i < jsonSessionArray.count(); i++) {
        expectedSessionList = expectedSessionList + ";" + jsonSessionArray.at(i).toString();
    }
    QCOMPARE(receivedSessionList, expectedSessionList);
}

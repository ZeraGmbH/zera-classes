#include "test_read_lock_state.h"
#include "scpimoduleclientblocked.h"
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_read_lock_state)

static int constexpr semEntityId = 1200;

void test_read_lock_state::cleanup()
{
    m_veinEventDump = QJsonObject();
}

void test_read_lock_state::readEmobPushButtonValue()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    VeinStorage::AbstractDatabase *dataBase = veinStorage->getDb();
    QList<int> entityList = dataBase->getEntityList();
    QVERIFY(entityList.contains(semEntityId));

    QVERIFY(dataBase->hasStoredValue(semEntityId, "PAR_EmobPushButton"));
    QVariant pressButtonValue = dataBase->getStoredValue(semEntityId, "PAR_EmobPushButton");
    QCOMPARE(pressButtonValue.toString(), "0");
}

void test_read_lock_state::pressAndReadEmobPushButtonValue()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    VeinStorage::AbstractDatabase *dataBase = veinStorage->getDb();

    QVariant oldValue = dataBase->getStoredValue(semEntityId, "PAR_EmobPushButton");
    QCOMPARE(oldValue, 0);

    testRunner.setVfComponent(semEntityId, "PAR_EmobPushButton", 1);
    QCOMPARE(dataBase->getStoredValue(semEntityId, "PAR_EmobPushButton"), 0);
}

void test_read_lock_state::readLockStateWrongRpcName()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    setupSpy(testRunner);

    ScpiModuleClientBlocked client;
    QString status = client.sendReceive("CALCULATE:EM01:0001:FOO?");
    QCOMPARE(status, "");

    QFile file(":/vein-event-dumps/dumpReadLockStateWrongRpcName.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_lock_state::readLockStateCorrectRpcName()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    setupSpy(testRunner);

    ScpiModuleClientBlocked client;
    QString status = client.sendReceive("CALCULATE:EM01:0001:EMLOCKSTATE?");
    QCOMPARE(status, "4");

    QFile file(":/vein-event-dumps/dumpReadLockStateCorrectRpcName.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_lock_state::readLockStateTwice()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    setupSpy(testRunner);

    ScpiModuleClientBlocked client;
    QString status1 = client.sendReceive("CALCULATE:EM01:0001:EMLOCKSTATE?");
    QString status2 = client.sendReceive("CALCULATE:EM01:0001:EMLOCKSTATE?");
    QCOMPARE(status1, "4");
    QCOMPARE(status1, status2);

    QFile file(":/vein-event-dumps/dumpReadLockStateTwice.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_lock_state::setupSpy(ModuleManagerTestRunner &modmanRunner)
{
    ModuleManagerSetupFacade* modManFacade = modmanRunner.getModManFacade();
    m_serverCmdEventSpyTop = std::make_unique<TestJsonSpyEventSystem>(&m_veinEventDump, "server-enter");
    modManFacade->prependModuleSystem(m_serverCmdEventSpyTop.get());

    m_serverCmdEventSpyBottom = std::make_unique<TestJsonSpyEventSystem>(&m_veinEventDump, "server-fallthrough");
    connect(modManFacade->getEventHandler(), &VeinEvent::EventHandler::sigEventAccepted,
            m_serverCmdEventSpyBottom.get(), &TestJsonSpyEventSystem::onEventAccepted);
    modManFacade->addSubsystem(m_serverCmdEventSpyBottom.get());
}

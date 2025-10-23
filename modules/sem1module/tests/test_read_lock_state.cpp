#include "test_read_lock_state.h"
#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QTest>

QTEST_MAIN(test_read_lock_state)

static int constexpr semEntityId = 1200;

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
    ScpiModuleClientBlocked client;
    QString status = client.sendReceive("CALCULATE:EM01:0001:RPC_READLOCKSTATEFOO()?");
    QCOMPARE(status, "");
}

void test_read_lock_state::readLockStateCorrectRpcName()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    ScpiModuleClientBlocked client;
    QString status = client.sendReceive("CALCULATE:EM01:0001:RPC_READLOCKSTATE()?");
    QCOMPARE(status, "4");
}

void test_read_lock_state::readLockStateTwice()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    ZeraModules::VirtualModule *module = testRunner.getModule("SEM1Module", semEntityId);
    ScpiModuleClientBlocked client;
    QString status1 = client.sendReceive("CALCULATE:EM01:0001:RPC_READLOCKSTATE()?");
    QString status2 = client.sendReceive("CALCULATE:EM01:0001:RPC_READLOCKSTATE()?");
    QCOMPARE(status1, status2);
}

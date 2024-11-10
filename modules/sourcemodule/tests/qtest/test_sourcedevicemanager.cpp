#include "test_sourcedevicemanager.h"
#include "test_globals.h"
#include "sourcedevicemanager.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"

QTEST_MAIN(test_sourcedevicemanager)

FinishEntry::FinishEntry(int slotNo, QUuid uuid, QString errorMsg)
{
    this->slotNo = slotNo;
    this->uuid = uuid;
    this->errorMsg = errorMsg;
}

void test_sourcedevicemanager::onSourceScanFinished(int slotNo, QUuid uuid, QString errorMsg)
{
    m_listSourcesAdded.append(FinishEntry(slotNo, uuid, errorMsg));
}

void test_sourcedevicemanager::onSlotRemoved(int slotNo)
{
    m_socketsRemoved.append(slotNo);
}

void test_sourcedevicemanager::init()
{
    m_listSourcesAdded.clear();
    m_socketsRemoved.clear();
    TimerFactoryQtForTest::enableTest();
}

void test_sourcedevicemanager::initSlotCount()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    checkSlotCount(devMan, slotCount, 0, 0);
}

void test_sourcedevicemanager::removeSlotsOnEmpty()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    int slotNoReceived = 0;
    int slotRemoveReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotRemoveReceived++;
        slotNoReceived = slotNo;
    });
    devMan.closeSource(0, QUuid());
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(slotRemoveReceived, 1);
    QCOMPARE(slotNoReceived, -1);
}

void test_sourcedevicemanager::removeSlotInvalidBelow()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    int slotNoReceived = 0;
    int slotRemoveReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotRemoveReceived++;
        slotNoReceived = slotNo;
    });
    devMan.closeSource(-1, QUuid());
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(slotRemoveReceived, 1);
    QCOMPARE(slotNoReceived, -1);
}

void test_sourcedevicemanager::removeSlotInvalidAbove()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    int slotNoReceived = 0;
    int slotRemoveReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotRemoveReceived++;
        slotNoReceived = slotNo;
    });
    devMan.closeSource(slotCount, QUuid());
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(slotRemoveReceived, 1);
    QCOMPARE(slotNoReceived, -1);
}

void test_sourcedevicemanager::removeTooManySlots()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    int validSlotNoReceived = 0;
    int invalidSlotNoReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        if(slotNo >= 0 && slotNo < slotCount) {
            validSlotNoReceived++;
        }
        else {
            invalidSlotNoReceived++;
        }
    });
    for(int i=0; i<slotCount; i++) {
        devMan.closeSource(i, QUuid());
        devMan.closeSource(i, QUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(validSlotNoReceived, slotCount);
    QCOMPARE(invalidSlotNoReceived, slotCount);
}

void test_sourcedevicemanager::removeDevInfoUnknown()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QUuid uuid = QUuid::createUuid();
    int slotReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotReceived = slotNo;
    });
    devMan.closeSource(QString("Foo"), uuid);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(slotReceived, -1);
}

void test_sourcedevicemanager::removeDevInfoUuidIdentical()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    QUuid uuidsReceived[slotCount];
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        uuidsReceived[slotNo] = uuidParam;
    });
    QUuid uuidsSet[slotCount];
    for(int i=0; i<slotCount; i++) {
        uuidsSet[i] = QUuid::createUuid();
        devMan.closeSource(i, uuidsSet[i]);
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(uuidsSet[0], uuidsReceived[0]);
    QCOMPARE(uuidsSet[1], uuidsReceived[1]);
    QCOMPARE(uuidsSet[2], uuidsReceived[2]);
}

void test_sourcedevicemanager::noSingleSignalRemoveAllOnEmpty()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);

    int singleRemoveCount = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int, QUuid) {
        singleRemoveCount++;
    });
    devMan.closeAll();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(singleRemoveCount, 0);
}

void test_sourcedevicemanager::noSingleSignalRemoveAllOnSlots()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

    int singleRemoveCount = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int, QUuid) {
        singleRemoveCount++;
    });
    devMan.closeAll();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(singleRemoveCount, slotCount);
}

void test_sourcedevicemanager::removeAllSignalOnEmpty()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);

    int allRemoveCount = 0;
    connect(&devMan, &SourceDeviceManager::sigAllSlotsRemoved, [&]() {
        allRemoveCount++;
    });
    devMan.closeAll();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(allRemoveCount, 1);
}

void test_sourcedevicemanager::removeAllSignalOnSlots()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

    int allRemoveCount = 0;
    connect(&devMan, &SourceDeviceManager::sigAllSlotsRemoved, [&]() {
        allRemoveCount++;
    });
    devMan.closeAll();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(allRemoveCount, 1);
}

void test_sourcedevicemanager::demoScanOne()
{
    SourceDeviceManager devMan(1);
    devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    TimeMachineForTest::getInstance()->processTimers(50);
    checkSlotCount(devMan, 1, 1, 1);
}

void test_sourcedevicemanager::demoScanTooMany()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; ++i) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    checkSlotCount(devMan, slotCount, slotCount, slotCount);
}

void test_sourcedevicemanager::demoScanAll()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    checkSlotCount(devMan, slotCount, slotCount, slotCount);
}

void test_sourcedevicemanager::demoScanAllAndRemove()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    for(int i=0; i<slotCount; i++) {
        devMan.closeSource(i, QUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    checkSlotCount(devMan, slotCount, 0, 0);
}

void test_sourcedevicemanager::demoAddNotification()
{
    checkAddRemoveNotifications(3, 3, 0);
}

void test_sourcedevicemanager::demoAddNotificationTooMany()
{
    constexpr int slotCount = 3;
    checkAddRemoveNotifications(slotCount, slotCount+1, 0);

    QCOMPARE(m_listSourcesAdded[slotCount].slotNo, -1);
    QVERIFY(!m_listSourcesAdded[slotCount].errorMsg.isEmpty());
}

void test_sourcedevicemanager::demoRemoveNotification()
{
    constexpr int slotCount = 3;
    checkAddRemoveNotifications(slotCount, slotCount, slotCount);
}

void test_sourcedevicemanager::demoRemoveNotificationTooMany()
{
    constexpr int slotCount = 3;
    checkAddRemoveNotifications(slotCount, slotCount, slotCount*2);
}

void test_sourcedevicemanager::demoScanNoDevFoundOnBroken()
{
    SourceDeviceManager devMan(2);
    connect(&devMan, &SourceDeviceManager::sigSourceScanFinished,
            this, &test_sourcedevicemanager::onSourceScanFinished);
    devMan.startSourceScan(IoDeviceTypes::BROKEN, "", QUuid::createUuid());

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(m_listSourcesAdded.count(), 1);
    QCOMPARE(m_listSourcesAdded[0].slotNo, -1);
}

void test_sourcedevicemanager::demoScanNoDevFoundOnDemo()
{
    SourceDeviceManager devMan(2);
    connect(&devMan, &SourceDeviceManager::sigSourceScanFinished,
            this, &test_sourcedevicemanager::onSourceScanFinished);
    devMan.startSourceScan(IoDeviceTypes::DEMO, "broken", QUuid::createUuid());

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(m_listSourcesAdded.count(), 1);
    QCOMPARE(m_listSourcesAdded[0].slotNo, -1);
}

void test_sourcedevicemanager::checkSlotCount(SourceDeviceManager& devMan, int total, int active, int demo)
{
    QCOMPARE(devMan.getSlotCount(), total);
    QCOMPARE(devMan.getActiveSlotCount(), active);
    QCOMPARE(devMan.getDemoCount(), demo);
}

void test_sourcedevicemanager::checkAddRemoveNotifications(int total, int add, int remove)
{
    SourceDeviceManager devMan(total);
    connect(&devMan, &SourceDeviceManager::sigSourceScanFinished,
            this, &test_sourcedevicemanager::onSourceScanFinished);
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved,
            this, &test_sourcedevicemanager::onSlotRemoved);

    for(int i=0; i<add; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), 0);
    for(int i=0; i<remove; i++) {
        devMan.closeSource(i, QUuid());
    }
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), remove);
}


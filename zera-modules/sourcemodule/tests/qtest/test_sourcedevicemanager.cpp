#include "test_sourcedevicemanager.h"
#include "test_globals.h"
#include "source-device/sourcedevicemanager.h"

QTEST_MAIN(test_sourcedevicemanager);

FinishEntry::FinishEntry(int slotNo, QUuid uuid, QString errMsg)
{
    this->slotNo = slotNo;
    this->uuid = uuid;
    this->errMsg = errMsg;
}

void test_sourcedevicemanager::onSourceScanFinished(int slotNo, QUuid uuid, QString errMsg)
{
    m_listSourcesAdded.append(FinishEntry(slotNo, uuid, errMsg));
}

void test_sourcedevicemanager::onSlotRemoved(int slotNo)
{
    m_socketsRemoved.append(slotNo);
}

void test_sourcedevicemanager::init()
{
    m_listSourcesAdded.clear();
    m_socketsRemoved.clear();
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
    QTest::qWait(shortQtEventTimeout);
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
    QTest::qWait(shortQtEventTimeout);
    int slotNoReceived = 0;
    int slotRemoveReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotRemoveReceived++;
        slotNoReceived = slotNo;
    });
    devMan.closeSource(-1, QUuid());
    QTest::qWait(shortQtEventTimeout);
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
    QTest::qWait(shortQtEventTimeout);
    int slotNoReceived = 0;
    int slotRemoveReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotRemoveReceived++;
        slotNoReceived = slotNo;
    });
    devMan.closeSource(slotCount, QUuid());
    QTest::qWait(shortQtEventTimeout);
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
    QTest::qWait(shortQtEventTimeout);
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
    QTest::qWait(shortQtEventTimeout);
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
    QTest::qWait(shortQtEventTimeout);
    QUuid uuid = QUuid::createUuid();
    int slotReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotReceived = slotNo;
    });
    devMan.closeSource(QString("Foo"), uuid);
    QTest::qWait(shortQtEventTimeout);
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
    QTest::qWait(shortQtEventTimeout);
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        uuidsReceived[slotNo] = uuidParam;
    });
    QUuid uuidsSet[slotCount];
    for(int i=0; i<slotCount; i++) {
        uuidsSet[i] = QUuid::createUuid();
        devMan.closeSource(i, uuidsSet[i]);
    }
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(uuidsSet[0], uuidsReceived[0]);
    QCOMPARE(uuidsSet[1], uuidsReceived[1]);
    QCOMPARE(uuidsSet[2], uuidsReceived[2]);
}

void test_sourcedevicemanager::removeAllSignalOnEmpty()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);

    int singleRemoveCount = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int, QUuid) {
        singleRemoveCount++;
    });
    int allRemoveCount = 0;
    connect(&devMan, &SourceDeviceManager::sigAllSlotsRemoved, [&]() {
        allRemoveCount++;
    });
    devMan.closeAll();
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(singleRemoveCount, 0);
    QCOMPARE(allRemoveCount, 1);
}

void test_sourcedevicemanager::removeAllSignalOnSlots()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(shortQtEventTimeout);

    int singleRemoveCount = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int, QUuid) {
        singleRemoveCount++;
    });
    int allRemoveCount = 0;
    connect(&devMan, &SourceDeviceManager::sigAllSlotsRemoved, [&]() {
        allRemoveCount++;
    });
    devMan.closeAll();
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(singleRemoveCount, slotCount);
    QCOMPARE(allRemoveCount, 1);
}

void test_sourcedevicemanager::demoScanOne()
{
    SourceDeviceManager devMan(1);
    devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    QTest::qWait(50);
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
    QTest::qWait(shortQtEventTimeout);
    checkSlotCount(devMan, slotCount, slotCount, slotCount);
}

void test_sourcedevicemanager::demoScanAll()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(shortQtEventTimeout);
    checkSlotCount(devMan, slotCount, slotCount, slotCount);
}

void test_sourcedevicemanager::demoScanAllAndRemove()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(shortQtEventTimeout);
    for(int i=0; i<slotCount; i++) {
        devMan.closeSource(i, QUuid());
    }
    QTest::qWait(shortQtEventTimeout);
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
    QVERIFY(!m_listSourcesAdded[slotCount].errMsg.isEmpty());
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

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_listSourcesAdded.count(), 1);
    QCOMPARE(m_listSourcesAdded[0].slotNo, -1);
}

void test_sourcedevicemanager::demoScanNoDevFoundOnDemo()
{
    SourceDeviceManager devMan(2);
    connect(&devMan, &SourceDeviceManager::sigSourceScanFinished,
            this, &test_sourcedevicemanager::onSourceScanFinished);
    devMan.startSourceScan(IoDeviceTypes::DEMO, "broken", QUuid::createUuid());

    QTest::qWait(shortQtEventTimeout);
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
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), 0);
    for(int i=0; i<remove; i++) {
        devMan.closeSource(i, QUuid());
    }
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), 0);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), remove);
}


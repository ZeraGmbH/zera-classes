#include "main-unittest-qt.h"
#include "qt-unittest-sourcedevicemanager.h"
#include "source-device/sourcedevicemanager.h"
#include "sourcescanner.h"

static QObject* pSourceDeviceManagerTest = addTest(new SourceDeviceManagerTest);

FinishEntry::FinishEntry(int slotNo, SourceDeviceVein *device, QUuid uuid, QString errMsg)
{
    this->slotNo = slotNo;
    this->device = device;
    this->uuid = uuid;
    this->errMsg = errMsg;
}

void SourceDeviceManagerTest::onSourceScanFinished(int slotNo, SourceDeviceVein *device, QUuid uuid, QString errMsg)
{
    m_listSourcesAdded.append(FinishEntry(slotNo, device, uuid, errMsg));
}

void SourceDeviceManagerTest::onSlotRemoved(int slotNo)
{
    m_socketsRemoved.append(slotNo);
}

void SourceDeviceManagerTest::init()
{
    m_listSourcesAdded.clear();
    m_socketsRemoved.clear();
}

void SourceDeviceManagerTest::initSlotCount()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    checkSlotCount(devMan, slotCount, 0, 0);
}

void SourceDeviceManagerTest::removeSlotsOnEmpty()
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
    QTest::qWait(10);
    QCOMPARE(slotRemoveReceived, 1);
    QCOMPARE(slotNoReceived, -1);
}

void SourceDeviceManagerTest::removeSlotInvalidBelow()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    int slotNoReceived = 0;
    int slotRemoveReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotRemoveReceived++;
        slotNoReceived = slotNo;
    });
    devMan.closeSource(-1, QUuid());
    QTest::qWait(10);
    QCOMPARE(slotRemoveReceived, 1);
    QCOMPARE(slotNoReceived, -1);
}

void SourceDeviceManagerTest::removeSlotInvalidAbove()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    int slotNoReceived = 0;
    int slotRemoveReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotRemoveReceived++;
        slotNoReceived = slotNo;
    });
    devMan.closeSource(slotCount, QUuid());
    QTest::qWait(10);
    QCOMPARE(slotRemoveReceived, 1);
    QCOMPARE(slotNoReceived, -1);
}

void SourceDeviceManagerTest::removeTooManySlots()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
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
    QTest::qWait(10);
    QCOMPARE(validSlotNoReceived, slotCount);
    QCOMPARE(invalidSlotNoReceived, slotCount);
}

void SourceDeviceManagerTest::removeDevInfoUnknown()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    QUuid uuid = QUuid::createUuid();
    int slotReceived = 0;
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        Q_UNUSED(uuidParam)
        slotReceived = slotNo;
    });
    devMan.closeSource(QString("Foo"), uuid);
    QTest::qWait(10);
    QCOMPARE(slotReceived, -1);
}

void SourceDeviceManagerTest::removeDevInfoUuidIdentical()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    QUuid uuidsReceived[slotCount];
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved, [&](int slotNo, QUuid uuidParam) {
        uuidsReceived[slotNo] = uuidParam;
    });
    QUuid uuidsSet[slotCount];
    for(int i=0; i<slotCount; i++) {
        uuidsSet[i] = QUuid::createUuid();
        devMan.closeSource(i, uuidsSet[i]);
    }
    QTest::qWait(10);
    QCOMPARE(uuidsSet[0], uuidsReceived[0]);
    QCOMPARE(uuidsSet[1], uuidsReceived[1]);
    QCOMPARE(uuidsSet[2], uuidsReceived[2]);
}

void SourceDeviceManagerTest::demoScanOne()
{
    SourceDeviceManager devMan(1);
    devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    QTest::qWait(50);
    checkSlotCount(devMan, 1, 1, 1);
}

void SourceDeviceManagerTest::demoScanTooMany()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; ++i) {
        devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    QTest::qWait(10);
    checkSlotCount(devMan, slotCount, slotCount, slotCount);
}

void SourceDeviceManagerTest::demoScanAll()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    checkSlotCount(devMan, slotCount, slotCount, slotCount);
}

void SourceDeviceManagerTest::demoScanAllAndRemove()
{
    constexpr int slotCount = 3;
    SourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    for(int i=0; i<slotCount; i++) {
        devMan.closeSource(i, QUuid());
    }
    QTest::qWait(10);
    checkSlotCount(devMan, slotCount, 0, 0);
}

void SourceDeviceManagerTest::demoAddNotification()
{
    checkAddRemoveNotifications(3, 3, 0);
}

void SourceDeviceManagerTest::demoAddNotificationTooMany()
{
    constexpr int slotCount = 3;
    checkAddRemoveNotifications(slotCount, slotCount+1, 0);

    QCOMPARE(m_listSourcesAdded[slotCount].slotNo, -1);
    QVERIFY(!m_listSourcesAdded[slotCount].errMsg.isEmpty());
}

void SourceDeviceManagerTest::demoRemoveNotification()
{
    constexpr int slotCount = 3;
    checkAddRemoveNotifications(slotCount, slotCount, slotCount);
}

void SourceDeviceManagerTest::demoRemoveNotificationTooMany()
{
    constexpr int slotCount = 3;
    checkAddRemoveNotifications(slotCount, slotCount, slotCount*2);
}

void SourceDeviceManagerTest::noCrashOnManagerDeadBeforeScanFinished()
{
    constexpr int slotCount = 3;
    SourceDeviceManager* devMan = new SourceDeviceManager(slotCount);
    connect(devMan, &SourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);
    for(int i=0; i<slotCount; i++) {
        devMan->startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    delete devMan;
    QCOMPARE(SourceScanner::getInstanceCount(), slotCount);
    QTest::qWait(10);
    QCOMPARE(m_listSourcesAdded.count(), 0);
    QCOMPARE(SourceScanner::getInstanceCount(), 0);
}

void SourceDeviceManagerTest::checkSlotCount(SourceDeviceManager& devMan, int total, int active, int demo)
{
    QCOMPARE(devMan.getSlotCount(), total);
    QCOMPARE(devMan.getActiveSlotCount(), active);
    QCOMPARE(devMan.getDemoCount(), demo);
}

void SourceDeviceManagerTest::checkAddRemoveNotifications(int total, int add, int remove)
{
    SourceDeviceManager devMan(total);
    connect(&devMan, &SourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);
    connect(&devMan, &SourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);

    for(int i=0; i<add; i++) {
        devMan.startSourceScan(SERIAL_DEVICE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), 0);
    for(int i=0; i<remove; i++) {
        devMan.closeSource(i, QUuid());
    }
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), 0);
    QTest::qWait(10);
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), remove);
}


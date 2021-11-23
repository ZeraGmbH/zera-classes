#include "main-unittest-qt.h"
#include "qt-unittest-sourcedevicemanager.h"
#include "sourcedevicemanager.h"
#include "sourcescanner.h"

static QObject* pSourceDeviceManagerTest = addTest(new SourceDeviceManagerTest);

FinishEntry::FinishEntry(int slotNo, SOURCEMODULE::cSourceDevice *device, QUuid uuid, QString errMsg)
{
    this->slotNo = slotNo;
    this->device = device;
    this->uuid = uuid;
    this->errMsg = errMsg;
}

void SourceDeviceManagerTest::onSourceScanFinished(int slotNo, SOURCEMODULE::cSourceDevice *device, QUuid uuid, QString errMsg)
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
    cSourceDeviceManager devMan(slotCount);
    checkSlotCount(devMan, slotCount, 0, 0);
}

void SourceDeviceManagerTest::removeSlotsOnEmpty()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        QCOMPARE(devMan.removeSource(i), false);
    }
}

void SourceDeviceManagerTest::removeSlotInvalidBelow()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    QCOMPARE(devMan.removeSource(-1), false);
}

void SourceDeviceManagerTest::removeSlotInvalidAbove()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    QCOMPARE(devMan.removeSource(slotCount), false);
}

void SourceDeviceManagerTest::removeTooManySlots()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    for(int i=0; i<slotCount; i++) {
        QCOMPARE(devMan.removeSource(i), true);
        QCOMPARE(devMan.removeSource(i), false);
    }
}

void SourceDeviceManagerTest::demoScanOne()
{
    cSourceDeviceManager devMan(1);
    devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    QTest::qWait(50);
    checkSlotCount(devMan, 1, 1, 1);
}

void SourceDeviceManagerTest::demoScanTooMany()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; ++i) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    QTest::qWait(10);
    checkSlotCount(devMan, slotCount, slotCount, slotCount);
}

void SourceDeviceManagerTest::demoScanAll()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    checkSlotCount(devMan, slotCount, slotCount, slotCount);
}

void SourceDeviceManagerTest::demoScanAllAndRemove()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    for(int i=0; i<slotCount; i++) {
        devMan.removeSource(i);
    }
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
    cSourceDeviceManager* devMan = new cSourceDeviceManager(slotCount);
    connect(devMan, &cSourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);
    for(int i=0; i<slotCount; i++) {
        devMan->startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    delete devMan;
    QCOMPARE(cSourceScanner::getInstanceCount(), slotCount);
    QTest::qWait(10);
    QCOMPARE(m_listSourcesAdded.count(), 0);
    QCOMPARE(cSourceScanner::getInstanceCount(), 0);
}

void SourceDeviceManagerTest::checkSlotCount(cSourceDeviceManager& devMan, int total, int active, int demo)
{
    QCOMPARE(devMan.getSlotCount(), total);
    QCOMPARE(devMan.getActiveSlotCount(), active);
    QCOMPARE(devMan.getDemoCount(), demo);
}

void SourceDeviceManagerTest::checkAddRemoveNotifications(int total, int add, int remove)
{
    cSourceDeviceManager devMan(total);
    connect(&devMan, &cSourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);
    connect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);

    for(int i=0; i<add; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), 0);

    for(int i=0; i<remove; i++) {
        devMan.removeSource(i);
    }
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), 0);
    QTest::qWait(10);
    QCOMPARE(m_listSourcesAdded.count(), add);
    QCOMPARE(m_socketsRemoved.count(), remove > total ? total : remove);
}


#include "main-unittest-qt.h"
#include "qt-unittest-sourcedevicemanager.h"
#include "sourcedevicemanager.h"
#include "sourcescanner.h"

static QObject* pSourceDeviceManagerTest = addTest(new SourceDeviceManagerTest);

using SOURCEMODULE::cSourceDeviceManager;
using SOURCEMODULE::cSourceScanner;

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

void SourceDeviceManagerTest::testInitState()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    QCOMPARE(devMan.getSlotCount(), slotCount);
    QCOMPARE(devMan.getActiveSlotCount(), 0);
    QCOMPARE(devMan.getDemoCount(), 0);
}

void SourceDeviceManagerTest::testAddTooMany()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount+2; ++i) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    QCOMPARE(devMan.getActiveSlotCount(),slotCount);
    QCOMPARE(devMan.getDemoCount(), slotCount);
}

void SourceDeviceManagerTest::testRemoveOnEmpty()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        QCOMPARE(devMan.removeSource(i), false);
    }
}

void SourceDeviceManagerTest::testRemoveInvalidBelow()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    QCOMPARE(devMan.removeSource(-1), false);
}

void SourceDeviceManagerTest::testRemoveInvalidAbove()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    QCOMPARE(devMan.removeSource(slotCount), false);
}

void SourceDeviceManagerTest::testRemoveTooMany()
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

void SourceDeviceManagerTest::testAvailAndDemoCountAdd()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    QCOMPARE(devMan.getActiveSlotCount(),slotCount);
    QCOMPARE(devMan.getDemoCount(), slotCount);
}

void SourceDeviceManagerTest::testAvailAndDemoCountAddRemove()
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
    QCOMPARE(devMan.getActiveSlotCount(),0);
    QCOMPARE(devMan.getDemoCount(), 0);
}

void SourceDeviceManagerTest::testAddNotification()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    connect(&devMan, &cSourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);
    connect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    disconnect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);
    disconnect(&devMan, &cSourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);
    QCOMPARE(m_listSourcesAdded.count(), slotCount);
    QCOMPARE(m_socketsRemoved.count(), 0);
}

void SourceDeviceManagerTest::testAddNotificationTooMany()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    connect(&devMan, &cSourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);
    connect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);
    for(int i=0; i<slotCount+1; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    disconnect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);
    disconnect(&devMan, &cSourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);
    QCOMPARE(m_listSourcesAdded.count(), slotCount+1);
    QCOMPARE(m_listSourcesAdded[slotCount].slotNo, -1);
    QVERIFY(!m_listSourcesAdded[slotCount].errMsg.isEmpty());
    QCOMPARE(m_socketsRemoved.count(), 0);
}

void SourceDeviceManagerTest::testRemoveNotification()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    connect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    for(int i=0; i<slotCount; i++) {
        devMan.removeSource(i);
    }
    QTest::qWait(10);
    disconnect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);

    QCOMPARE(m_socketsRemoved.count(), slotCount);
}

void SourceDeviceManagerTest::testRemoveNotificationTooMany()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    connect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    for(int i=0; i<slotCount; i++) {
        devMan.removeSource(i);
        devMan.removeSource(i);
    }
    QTest::qWait(10);
    disconnect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);

    QCOMPARE(m_socketsRemoved.count(), slotCount);
}

void SourceDeviceManagerTest::testAddRemoveNotificationAndCounts()
{
    constexpr int slotCount = 3;
    cSourceDeviceManager devMan(slotCount);
    connect(&devMan, &cSourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);
    connect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);
    for(int i=0; i<slotCount; i++) {
        devMan.startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
    }
    QTest::qWait(10);
    QCOMPARE(m_listSourcesAdded.count(), slotCount);
    QCOMPARE(m_socketsRemoved.count(), 0);
    QCOMPARE(devMan.getActiveSlotCount(), slotCount);
    QCOMPARE(devMan.getDemoCount(), slotCount);
    for(int i=0; i<slotCount+1; i++) {
        devMan.removeSource(i);
    }
    QTest::qWait(10);
    disconnect(&devMan, &cSourceDeviceManager::sigSlotRemoved,
            this, &SourceDeviceManagerTest::onSlotRemoved);
    disconnect(&devMan, &cSourceDeviceManager::sigSourceScanFinished,
            this, &SourceDeviceManagerTest::onSourceScanFinished);

    QCOMPARE(m_socketsRemoved.count(), slotCount);
    QCOMPARE(devMan.getActiveSlotCount(), 0);
    QCOMPARE(devMan.getDemoCount(), 0);
}

void SourceDeviceManagerTest::testNoCrashOnManagerDeadBeforeScanFinished()
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



#include <gtest/gtest.h>
#include <QTimer>
#include <QCoreApplication>
#include "sourcedevicemanager.h"
#include "sourceinterface.h"
#include "sourcedevice.h"

static int goodAdded = 0;
static int badAdded = 0;
static int removed = 0;

#define DeviceManager SOURCEMODULE::cSourceDeviceManager
#define SourceDevice SOURCEMODULE::cSourceDevice

static void testWithEvents(DeviceManager* devManager, int countAddGood, int countAddBad, int countRemove, int countAddPost = 0)
{
    goodAdded = 0;
    badAdded = 0;
    removed = 0;

    char const *p = "abc";
    char const **dummyParam = { &p };
    int argc = 1;
    QCoreApplication loop(argc, const_cast<char**>(dummyParam));

    QTimer timerForEventLoopSetup;
    timerForEventLoopSetup.setSingleShot(true);
    QTimer timerForFinish;
    timerForFinish.setSingleShot(true);

    bool postAddRequired = countAddPost > 0;

    QObject::connect(devManager, &DeviceManager::sigSourceScanFinished, [&](int slotNo, SourceDevice* device, QUuid uuid, QString errMsg) {
        Q_UNUSED(slotNo)
        Q_UNUSED(uuid)
        Q_UNUSED(errMsg)
        if(device) {
            goodAdded++;
        }
        else {
            badAdded++;
        }
        for(int slotNo=0; slotNo<devManager->slotCount() && countRemove; slotNo++) {
            if(devManager->removeSource(slotNo)) {
                countRemove--;
            }
        }
    });
    QObject::connect(devManager, &DeviceManager::sigSlotRemoved, [&](int slotNo) {
        Q_UNUSED(slotNo)
        removed++;
    });

    QObject::connect( &timerForFinish, &QTimer::timeout, [&]() {
        if(!postAddRequired) {
            loop.quit();
        }
        else {
            postAddRequired = false;
            for(int i=0; i<countAddPost; i++) {
                devManager->startSourceScan(SOURCE_INTERFACE_DEMO, "", QUuid::createUuid());
            }
            timerForFinish.start(200);
        }
    });
    QObject::connect( &timerForEventLoopSetup, &QTimer::timeout, [&]() {
        // We have a running event loop here start our test
        timerForFinish.start(200);
        int i;
        for(i=0; i<countAddGood; i++) {
            devManager->startSourceScan(SOURCE_INTERFACE_DEMO, "", QUuid::createUuid());
        }
        for(i=0; i<countAddBad; i++) {
            devManager->startSourceScan(SOURCE_INTERFACE_BASE, "", QUuid::createUuid());
        }
    });
    timerForEventLoopSetup.start(0);
    loop.exec();
}

TEST(TEST_SOURCE_DEVICE_MANAGER, CREATE_DEMO_GOOD) {
    DeviceManager devManager(2);
    testWithEvents(&devManager, 2, 0, 0);
    EXPECT_EQ(goodAdded, 2);
    EXPECT_EQ(badAdded, 0);
    EXPECT_EQ(removed, 0);
    EXPECT_EQ(devManager.activeSlotCount(), 2);
    EXPECT_EQ(devManager.demoCount(), 2);
}

TEST(TEST_SOURCE_DEVICE_MANAGER, CREATE_BASE_BAD) {
    DeviceManager devManager(2);
    testWithEvents(&devManager, 0, 2, 0);
    EXPECT_EQ(goodAdded, 0);
    EXPECT_EQ(badAdded, 2);
    EXPECT_EQ(removed, 0);
    EXPECT_EQ(devManager.activeSlotCount(), 0);
    EXPECT_EQ(devManager.demoCount(), 0);
}

TEST(TEST_SOURCE_DEVICE_MANAGER, CREATE_MORE_THAN_SLOTS) {
    DeviceManager devManager(2);
    testWithEvents(&devManager, 3, 0, 0);
    EXPECT_EQ(goodAdded, 2);
    EXPECT_EQ(badAdded, 1);
    EXPECT_EQ(removed, 0);
    EXPECT_EQ(devManager.activeSlotCount(), 2);
    EXPECT_EQ(devManager.demoCount(), 2);
}

TEST(TEST_SOURCE_DEVICE_MANAGER, CREATE_MORE_THAN_SLOTS_POST) {
    // We have two checks for slots full: At entry and after source was created
    // CREATE_MORE_THAN_SLOTS sends out all scans and the checks are performed
    // after creation. Here we check the entry by:
    DeviceManager devManager(2);
    testWithEvents(&devManager, 2, 0, 0, 2);
    EXPECT_EQ(goodAdded, 2);
    EXPECT_EQ(badAdded, 2);
    EXPECT_EQ(devManager.activeSlotCount(), 2);
    EXPECT_EQ(devManager.demoCount(), 2);
}

TEST(TEST_SOURCE_DEVICE_MANAGER, CREATE_DEMO_GOOD_AND_REMOVE) {
    DeviceManager devManager(2);
    testWithEvents(&devManager, 2, 0, 2);
    EXPECT_EQ(goodAdded, 2);
    EXPECT_EQ(badAdded, 0);
    EXPECT_EQ(removed, 2);
    EXPECT_EQ(devManager.activeSlotCount(), 0);
    EXPECT_EQ(devManager.demoCount(), 0);
}



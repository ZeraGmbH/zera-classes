#include <gtest/gtest.h>
#include <QTimer>
#include <QCoreApplication>
#include "sourcedevicemanager.h"
#include "sourceinterface.h"
#include "sourcedevice.h"

static int goodAdded = 0;
static int badAdded = 0;
static int removed = 0;

using SOURCEMODULE::cSourceDeviceManager;
using SOURCEMODULE::cSourceDevice;

static void testDeviceManagerEvents(cSourceDeviceManager* devManager,
                           int countAddGood, int countAddBad, int countRemove, int countAddPost = 0)
{
    goodAdded = 0;
    badAdded = 0;
    removed = 0;

    char const *p = "devicemanager-test";
    char const **dummyParam = { &p };
    int argc = 1;
    QCoreApplication loop(argc, const_cast<char**>(dummyParam));

    QTimer timerForEventLoopSetup;
    timerForEventLoopSetup.setSingleShot(true);
    QTimer timerForFinish;
    timerForFinish.setSingleShot(true);

    bool postAddRequired = countAddPost > 0;

    QObject::connect(devManager, &cSourceDeviceManager::sigSourceScanFinished,
                     [&](int, cSourceDevice* device, QUuid, QString) {
        if(device) {
            goodAdded++;
        }
        else {
            badAdded++;
        }
        for(int slotNo=0; slotNo<devManager->getSlotCount() && countRemove; slotNo++) {
            if(devManager->removeSource(slotNo)) {
                countRemove--;
            }
        }
    });
    QObject::connect(devManager, &cSourceDeviceManager::sigSlotRemoved, [&](int) {
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
    cSourceDeviceManager devManager(2);
    testDeviceManagerEvents(&devManager, 2, 0, 0);
    EXPECT_EQ(goodAdded, 2);
    EXPECT_EQ(badAdded, 0);
    EXPECT_EQ(removed, 0);
    EXPECT_EQ(devManager.getActiveSlotCount(), 2);
    EXPECT_EQ(devManager.getDemoCount(), 2);
}

TEST(TEST_SOURCE_DEVICE_MANAGER, CREATE_BASE_BAD) {
    cSourceDeviceManager devManager(2);
    testDeviceManagerEvents(&devManager, 0, 2, 0);
    EXPECT_EQ(goodAdded, 0);
    EXPECT_EQ(badAdded, 2);
    EXPECT_EQ(removed, 0);
    EXPECT_EQ(devManager.getActiveSlotCount(), 0);
    EXPECT_EQ(devManager.getDemoCount(), 0);
}

TEST(TEST_SOURCE_DEVICE_MANAGER, CREATE_MORE_THAN_SLOTS) {
    cSourceDeviceManager devManager(2);
    testDeviceManagerEvents(&devManager, 3, 0, 0);
    EXPECT_EQ(goodAdded, 2);
    EXPECT_EQ(badAdded, 1);
    EXPECT_EQ(removed, 0);
    EXPECT_EQ(devManager.getActiveSlotCount(), 2);
    EXPECT_EQ(devManager.getDemoCount(), 2);
}

TEST(TEST_SOURCE_DEVICE_MANAGER, CREATE_MORE_THAN_SLOTS_POST) {
    // We have two checks for slots full: At entry and after source was created
    // CREATE_MORE_THAN_SLOTS sends out all scans and the checks are performed
    // after creation. Here we check the entry by:
    cSourceDeviceManager devManager(2);
    testDeviceManagerEvents(&devManager, 2, 0, 0, 2);
    EXPECT_EQ(goodAdded, 2);
    EXPECT_EQ(badAdded, 2);
    EXPECT_EQ(devManager.getActiveSlotCount(), 2);
    EXPECT_EQ(devManager.getDemoCount(), 2);
}

TEST(TEST_SOURCE_DEVICE_MANAGER, CREATE_DEMO_GOOD_AND_REMOVE) {
    cSourceDeviceManager devManager(2);
    testDeviceManagerEvents(&devManager, 2, 0, 2);
    EXPECT_EQ(goodAdded, 2);
    EXPECT_EQ(badAdded, 0);
    EXPECT_EQ(removed, 2);
    EXPECT_EQ(devManager.getActiveSlotCount(), 0);
    EXPECT_EQ(devManager.getDemoCount(), 0);
}


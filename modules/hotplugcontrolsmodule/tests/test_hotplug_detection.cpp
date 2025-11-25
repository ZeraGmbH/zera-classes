#include "test_hotplug_detection.h"
#include "modulemanagertestrunner.h"
#include "controllerpersitentdata.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_hotplug_detection)

static int constexpr hotplugControlsEntityId = 1700;
static constexpr int statusEntityId = 1150;
static int constexpr serverPort = 4711;

void test_hotplug_detection::cleanup()
{
    ControllerPersitentData::cleanupPersitentData();
}

void test_hotplug_detection::noHotplug()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    QList<int> entityList = testRunner.getVeinStorageSystem()->getDb()->getEntityList();

    QCOMPARE(entityList.count(), 3);
    QVERIFY(entityList.contains(statusEntityId));
    QVERIFY(entityList.contains(hotplugControlsEntityId));

    QVariant ctrlersFound = testRunner.getVeinStorageSystem()->getDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
}

void test_hotplug_detection::oneEmobConnected()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    testRunner.fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant ctrlersFound = testRunner.getVeinStorageSystem()->getDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
}

void test_hotplug_detection::oneMt650eConnected()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IAUX", {"Mt650e", cClamp::undefined});
    testRunner.fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant ctrlersFound = testRunner.getVeinStorageSystem()->getDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
}

void test_hotplug_detection::twoEmobsConnected()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    infoMap.insert("IL2", {"EMOB", cClamp::undefined});
    testRunner.fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant ctrlersFound = testRunner.getVeinStorageSystem()->getDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
}

void test_hotplug_detection::twoMt650eConnected()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"Mt650e", cClamp::undefined});
    infoMap.insert("IL2", {"Mt650e", cClamp::undefined});
    testRunner.fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant ctrlersFound = testRunner.getVeinStorageSystem()->getDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
}

void test_hotplug_detection::oneEmobOneMt650eConnected()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    TimeMachineObject::feedEventLoop();
    QVariant ctrlersFound = testRunner.getVeinStorageSystem()->getDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);

    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    infoMap.insert("IL2", {"Mt650e", cClamp::undefined});
    testRunner.fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    ctrlersFound = testRunner.getVeinStorageSystem()->getDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , true);
}


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
    QList<int> entityList = testRunner.getVeinStorageDb()->getEntityList();

    QCOMPARE(entityList.count(), 4);
    QVERIFY(entityList.contains(statusEntityId));
    QVERIFY(entityList.contains(hotplugControlsEntityId));

    QVariant ctrlersFound = testRunner.getVeinStorageDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
}

void test_hotplug_detection::oneEmobConnected()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    testRunner.fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant ctrlersFound = testRunner.getVeinStorageDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
    int muxChannel = testRunner.getSenseSettings()->findChannelSettingByAlias1("IL1")->m_nMuxChannelNo;
    QVERIFY(ControllerPersitentData::getData().m_hotpluggedDevices[muxChannel].emobDataReceived[0].isEmpty());
}

void test_hotplug_detection::oneMt650eConnected()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IAUX", {"Mt650e", cClamp::undefined});
    testRunner.fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant ctrlersFound = testRunner.getVeinStorageDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
}

void test_hotplug_detection::twoEmobsConnected()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    infoMap.insert("IL2", {"EMOB", cClamp::undefined});
    testRunner.fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant ctrlersFound = testRunner.getVeinStorageDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
}

void test_hotplug_detection::twoMt650eConnected()
{
    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"Mt650e", cClamp::undefined});
    infoMap.insert("IL2", {"Mt650e", cClamp::undefined});
    testRunner.fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant ctrlersFound = testRunner.getVeinStorageDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
}

void test_hotplug_detection::oneEmobOneMt650eConnected()
{
    QByteArray emobExchangeData = createTestEmobExchangeData();
    int emobId = 0;

    ModuleManagerTestRunner testRunner(":/hotpluscontrols-status-session.json");
    TimeMachineObject::feedEventLoop();
    QVariant ctrlersFound = testRunner.getVeinStorageDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);

    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    infoMap.insert("IL2", {"Mt650e", cClamp::undefined});
    testRunner.fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    ctrlersFound = testRunner.getVeinStorageDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , true);
    int muxChannel = testRunner.getSenseSettings()->findChannelSettingByAlias1("IL1")->m_nMuxChannelNo;
    ControllerPersitentData::TPersitentControllerData foo = ControllerPersitentData::getData();
    QCOMPARE(emobExchangeData, ControllerPersitentData::getData().m_hotpluggedDevices[muxChannel].emobDataReceived[emobId]);
}

void test_hotplug_detection::oneEmobOneMt650eOnStartup()
{
    QByteArray emobExchangeData = createTestEmobExchangeData();
    int emobId = 0;

    ModuleManagerTestRunner testRunner("");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    infoMap.insert("IL2", {"Mt650e", cClamp::undefined});
    testRunner.fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();
    testRunner.start(":/hotpluscontrols-status-session.json");

    QVariant ctrlersFound = testRunner.getVeinStorageDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , true);
    int muxChannel = testRunner.getSenseSettings()->findChannelSettingByAlias1("IL1")->m_nMuxChannelNo;
    ControllerPersitentData::TPersitentControllerData foo = ControllerPersitentData::getData();
    QCOMPARE(emobExchangeData, ControllerPersitentData::getData().m_hotpluggedDevices[muxChannel].emobDataReceived[emobId]);
}

void test_hotplug_detection::oneEmobOnStartup()
{
    int emobId = 0;

    ModuleManagerTestRunner testRunner("");
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    testRunner.fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();
    testRunner.start(":/hotpluscontrols-status-session.json");

    QVariant ctrlersFound = testRunner.getVeinStorageDb()->getStoredValue(hotplugControlsEntityId, "ACT_ControllersFound");
    QCOMPARE(ctrlersFound , false);
    int muxChannel = testRunner.getSenseSettings()->findChannelSettingByAlias1("IL1")->m_nMuxChannelNo;
    ControllerPersitentData::TPersitentControllerData foo = ControllerPersitentData::getData();
    QVERIFY(ControllerPersitentData::getData().m_hotpluggedDevices[muxChannel].emobDataReceived[emobId].isEmpty());
}

QByteArray test_hotplug_detection::createTestEmobExchangeData()
{
    QByteArray emobExchangeData;
    emobExchangeData.resize(256);
    for(int i=0; i<emobExchangeData.size(); ++i)
        emobExchangeData[i] = i;
    return emobExchangeData;
}


#include "main-unittest-qt.h"
#include "qt-unittest-sourcedevicestatuswatcherjson.h"
#include "source-device/sourcedevicestatuswatcherjson.h"
#include "source-device/sourcedeviceswitcherjson.h"
#include "io-device/iodevicedemo.h"
#include "json/jsonstructureloader.h"

#include <zera-json-params-state.h>

static QObject* pSourceDeviceTest = addTest(new SourceDeviceStatusWatcherJsonTest);

SourceDeviceStatusWatcherJsonTest::~SourceDeviceStatusWatcherJsonTest()
{
    delete m_sourceDevice;
}

void SourceDeviceStatusWatcherJsonTest::init()
{
    delete m_sourceDevice;
    m_interface = nullptr;

    m_interface = createOpenDemoInterface();
    m_sourceDevice = new SourceDevice(m_interface, SOURCE_MT_COMMON, "", "");
}

void SourceDeviceStatusWatcherJsonTest::statusPollAutoStart()
{
    m_sourceDevice->setDemoResponseDelay(false, 0);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);
    int statePollSignalCount = 0;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] {
        statePollSignalCount++;
    });

    QTest::qWait(10);
    QVERIFY(statePollSignalCount > 0);
}

void SourceDeviceStatusWatcherJsonTest::statusPollChangeTime()
{
    m_sourceDevice->setDemoResponseDelay(false, 0);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(10);
    int statePollSignalCount = 0;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] {
        statePollSignalCount++;
    });

    QTest::qWait(5);
    QCOMPARE(statePollSignalCount, 0);
    QTest::qWait(20);
    QCOMPARE(statePollSignalCount, 1);
}

void SourceDeviceStatusWatcherJsonTest::initialBusyIdle()
{
    m_sourceDevice->setDemoResponseDelay(false, 0);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);
    int statePollSignalCount = 0;
    SOURCE_STATE stateReceived = SOURCE_STATE::UNDEFINED;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        stateReceived = state;
        statePollSignalCount++;
    });

    QTest::qWait(10);
    QCOMPARE(statePollSignalCount, 1);
    QCOMPARE(stateReceived, SOURCE_STATE::IDLE);
}

void SourceDeviceStatusWatcherJsonTest::busyOnCausesBusyOnOffState()
{
    m_sourceDevice->setDemoResponseDelay(false, 0);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);
    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    QTest::qWait(10); // ignore initial idle

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SOURCE_STATE::IDLE);
}

void SourceDeviceStatusWatcherJsonTest::busyOnOffCausesBusyTwoOnOffState()
{
    m_sourceDevice->setDemoResponseDelay(false, 0);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);
    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    QTest::qWait(10); // ignore initial idle

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);
    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SOURCE_STATE::IDLE);

    switcher.switchOff();
    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 4);
    QCOMPARE(statesReceived[2], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[3], SOURCE_STATE::IDLE);
}

void SourceDeviceStatusWatcherJsonTest::sequencePollSwitchErrorOnSwitch()
{
    m_sourceDevice->setDemoResponseDelay(false, 1);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);
    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    QTest::qWait(10); // ignore initial idle

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });

    m_sourceDevice->setDemoResonseError(true);
    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SOURCE_STATE::ERROR_SWITCH);
}

void SourceDeviceStatusWatcherJsonTest::sequencePollSwitchErrorOnPoll()
{
    m_sourceDevice->setDemoResponseDelay(false, 1);

    m_sourceDevice->setDemoResonseError(true);
    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });
    QTest::qWait(1); // start poll transaction

    m_sourceDevice->setDemoResonseError(false);
    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SOURCE_STATE::ERROR_POLL);
}

void SourceDeviceStatusWatcherJsonTest::sequenceSwitchPollErrorOnSwitch()
{
    m_sourceDevice->setDemoResponseDelay(false, 1);

    m_sourceDevice->setDemoResonseError(true);
    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    m_sourceDevice->setDemoResonseError(false);
    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SOURCE_STATE::ERROR_SWITCH);
}

void SourceDeviceStatusWatcherJsonTest::sequenceSwitchPollErrorOnPoll()
{
    m_sourceDevice->setDemoResponseDelay(false, 1);

    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    m_sourceDevice->setDemoResonseError(true);
    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 3);
    QCOMPARE(statesReceived[0], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SOURCE_STATE::IDLE);
    QCOMPARE(statesReceived[2], SOURCE_STATE::ERROR_POLL);
}

void SourceDeviceStatusWatcherJsonTest::sequencePollSwitchErrorOnBoth()
{
    m_sourceDevice->setDemoResponseDelay(false, 1);
    m_sourceDevice->setDemoResonseError(true);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);
    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    QTest::qWait(1); // start poll transaction

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SOURCE_STATE::ERROR_POLL);
}

void SourceDeviceStatusWatcherJsonTest::sequenceSwitchPollErrorOnBoth()
{
    m_sourceDevice->setDemoResponseDelay(false, 1);
    m_sourceDevice->setDemoResonseError(true);

    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SOURCE_STATE::ERROR_SWITCH);
}

void SourceDeviceStatusWatcherJsonTest::sequencePollStopsOnError()
{
    m_sourceDevice->setDemoResponseDelay(false, 0);
    m_sourceDevice->setDemoResonseError(true);

    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[1], SOURCE_STATE::ERROR_SWITCH);
    QCOMPARE(m_sourceDevice->isIoBusy(), false);

    statesReceived.clear();
    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 0);
    QCOMPARE(m_sourceDevice->isIoBusy(), false);
}

void SourceDeviceStatusWatcherJsonTest::sequencePollStopsOnErrorAndStartsOnSwitch()
{
    m_sourceDevice->setDemoResponseDelay(false, 1);
    m_sourceDevice->setDemoResonseError(true);

    SourceDeviceSwitcherJson switcher(m_sourceDevice);
    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    SourceDeviceStatusWatcherJson stateWatcher(m_sourceDevice);
    stateWatcher.setPollTime(0);

    QList<SOURCE_STATE> statesReceived;
    connect(&stateWatcher, &SourceDeviceStatusWatcherJson::sigStateChanged, [&] (SOURCE_STATE state) {
        statesReceived.append(state);
    });

    QTest::qWait(30);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[1], SOURCE_STATE::ERROR_SWITCH);
    QCOMPARE(m_sourceDevice->isIoBusy(), false);

    statesReceived.clear();
    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 0);
    QCOMPARE(m_sourceDevice->isIoBusy(), false);

    m_sourceDevice->setDemoResonseError(false);
    switcher.switchState(jsonParam);
    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SOURCE_STATE::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SOURCE_STATE::IDLE);
}


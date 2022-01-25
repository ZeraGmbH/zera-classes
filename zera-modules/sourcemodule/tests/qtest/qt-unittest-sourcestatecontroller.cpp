#include "main-unittest-qt.h"
#include "qt-unittest-sourcestatecontroller.h"
#include "source-device/sourcestatecontroller.h"
#include "source-device/sourceswitchjson.h"
#include "io-device/iodevicedemo.h"
#include "json/jsonstructureloader.h"

#include "sourcedeviceerrorinjection-forunittest.h"
#include <zera-json-params-state.h>

static QObject* pSourceDeviceTest = addTest(new SourceStateControllerTest);

SourceStateControllerTest::~SourceStateControllerTest()
{
    delete m_sourceDeviceUnderTest;
    delete m_sourceDevice;
}

void SourceStateControllerTest::onIoQueueGroupFinished(IoTransferDataGroup::Ptr workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void SourceStateControllerTest::init()
{
    delete m_sourceDeviceUnderTest;
    delete m_sourceDevice;
    m_ioDevice = nullptr;
    m_listIoGroupsReceived.clear();

    m_ioDevice = createOpenDemoIoDevice();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    m_sourceDeviceUnderTest = new SourceDevice(m_ioDevice, sourceProperties);
    m_sourceDevice = new SourceDeviceErrorInjection(m_sourceDeviceUnderTest);
    connect(m_sourceDevice, &ISourceDevice::sigResponseReceived,
            this, &SourceStateControllerTest::onIoQueueGroupFinished);
}

void SourceStateControllerTest::stateQueryIdle()
{
    SourceStateQueries state;
    state.setState(SourceStates::IDLE);
    QVERIFY(state.isIdle());
}

void SourceStateControllerTest::stateQueryError()
{
    SourceStateQueries state;
    state.setState(SourceStates::ERROR_POLL);
    QVERIFY(state.isError());
    state.setState(SourceStates::ERROR_SWITCH);
    QVERIFY(state.isError());
}

void SourceStateControllerTest::stateQueryBusy()
{
    SourceStateQueries state;
    state.setState(SourceStates::SWITCH_BUSY);
    QVERIFY(state.isSwitchBusy());
}

void SourceStateControllerTest::stateQueryUndefined()
{
    SourceStateQueries state;
    state.setState(SourceStates::UNDEFINED);
    QVERIFY(state.isUndefined());
}

void SourceStateControllerTest::stateQueryInitUndefined()
{
    SourceStateQueries state;
    QVERIFY(state.isUndefined());
}

void SourceStateControllerTest::stateQueryUnequal()
{
    SourceStateQueries state;
    state.setState(SourceStates::UNDEFINED);
    QVERIFY(state.isUnequal(SourceStates::IDLE));
}

void SourceStateControllerTest::statePollAutoStart()
{
    setDemoIoFixedTimeout(m_ioDevice, 0);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    stateWatcher.setPollTime(0);
    int statePollSignalCount = 0;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] {
        statePollSignalCount++;
    });

    QTest::qWait(10);
    QVERIFY(statePollSignalCount > 0);
    QVERIFY(stateWatcher.isPeriodicPollActive());
}

void SourceStateControllerTest::statePollChangeTime()
{
    setDemoIoFixedTimeout(m_ioDevice, 0);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    stateWatcher.setPollTime(10);
    int statePollSignalCount = 0;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] {
        statePollSignalCount++;
    });

    QTest::qWait(5);
    QCOMPARE(statePollSignalCount, 0);
    QTest::qWait(20);
    QCOMPARE(statePollSignalCount, 1);
}

void SourceStateControllerTest::stateInitialIdle()
{
    setDemoIoFixedTimeout(m_ioDevice, 0);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    stateWatcher.setPollTime(0);
    int statePollSignalCount = 0;
    SourceStates stateReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        stateReceived = state;
        statePollSignalCount++;
    });

    QTest::qWait(10);
    QCOMPARE(statePollSignalCount, 1);
    QCOMPARE(stateReceived, SourceStates::IDLE);
}

void SourceStateControllerTest::switchOnCausesBusyOnOffState()
{
    setDemoIoFixedTimeout(m_ioDevice, 0);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    stateWatcher.setPollTime(0);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);
    QTest::qWait(10); // ignore initial idle

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStates::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStates::IDLE);
}

void SourceStateControllerTest::switchOnOffCausesBusyTwoOnOffState()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    stateWatcher.setPollTime(0);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);
    QTest::qWait(10); // ignore initial idle

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);
    QTest::qWait(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStates::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStates::IDLE);

    switcher.switchOff();
    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 4);
    QCOMPARE(statesReceived[2], SourceStates::SWITCH_BUSY);
    QCOMPARE(statesReceived[3], SourceStates::IDLE);
}

void SourceStateControllerTest::sequencePollSwitchErrorOnSwitch()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    QVERIFY(stateWatcher.tryStartPollNow());
    m_sourceDevice->setDemoResonseError(true);
    switcher.switchState(jsonParam);

    QTest::qWait(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStates::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStates::ERROR_SWITCH);
}

void SourceStateControllerTest::sequencePollSwitchErrorOnPoll()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    m_sourceDevice->setDemoResonseError(true);
    QVERIFY(stateWatcher.tryStartPollNow());
    m_sourceDevice->setDemoResonseError(false);
    switcher.switchState(jsonParam);

    QTest::qWait(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStates::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStates::ERROR_POLL);
}

void SourceStateControllerTest::sequenceSwitchPollErrorOnSwitch()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    m_sourceDevice->setDemoResonseError(true);
    switcher.switchState(jsonParam);
    m_sourceDevice->setDemoResonseError(false);
    QVERIFY(stateWatcher.tryStartPollNow());

    QTest::qWait(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStates::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStates::ERROR_SWITCH);
}

void SourceStateControllerTest::sequenceSwitchPollErrorOnPoll()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    m_sourceDevice->setDemoResonseError(false);
    switcher.switchState(jsonParam);
    m_sourceDevice->setDemoResonseError(true);
    stateWatcher.setPollTime(1);

    QTest::qWait(100);
    QCOMPARE(statesReceived.count(), 3);
    QCOMPARE(statesReceived[0], SourceStates::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStates::IDLE);
    QCOMPARE(statesReceived[2], SourceStates::ERROR_POLL);
}

void SourceStateControllerTest::sequencePollSwitchErrorOnBoth()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    m_sourceDevice->setDemoResonseError(true);
    QVERIFY(stateWatcher.tryStartPollNow());
    switcher.switchState(jsonParam);

    QTest::qWait(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStates::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStates::ERROR_POLL);
}

void SourceStateControllerTest::sequenceSwitchPollErrorOnBoth()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    m_sourceDevice->setDemoResonseError(true);
    switcher.switchState(jsonParam);
    QVERIFY(stateWatcher.tryStartPollNow());

    QTest::qWait(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStates::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStates::ERROR_SWITCH);
}

void SourceStateControllerTest::pollStopsAfterSwitchError()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    m_sourceDevice->setDemoResonseError(true);
    switcher.switchState(jsonParam);
    stateWatcher.setPollTime(0);

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[1], SourceStates::ERROR_SWITCH);

    QVERIFY(!stateWatcher.isPeriodicPollActive());
}

void SourceStateControllerTest::pollStopsAfterPollError()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    m_sourceDevice->setDemoResonseError(true);
    stateWatcher.setPollTime(0);

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 1);
    QCOMPARE(statesReceived[0], SourceStates::ERROR_POLL);

    QVERIFY(!stateWatcher.isPeriodicPollActive());
}

void SourceStateControllerTest::pollStopsAfterErrorAndRestartsAfterSuccessfulSwitch()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier notifyWrapperSwitch(m_sourceDevice);
    SourceTransactionStartNotifier notifyWrapperState(m_sourceDevice);
    SourceStateController stateWatcher(m_sourceDevice, &notifyWrapperSwitch, &notifyWrapperState);
    SourceSwitchJson switcher(m_sourceDevice, &notifyWrapperSwitch);

    QList<SourceStates> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStates state) {
        statesReceived.append(state);
    });

    m_sourceDevice->setDemoResonseError(true);
    stateWatcher.setPollTime(0);

    QTest::qWait(10);
    QCOMPARE(statesReceived.count(), 1);
    QCOMPARE(statesReceived[0], SourceStates::ERROR_POLL);

    QVERIFY(!stateWatcher.isPeriodicPollActive());

    m_sourceDevice->setDemoResonseError(false);
    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    QVERIFY(stateWatcher.isPeriodicPollActive());
}


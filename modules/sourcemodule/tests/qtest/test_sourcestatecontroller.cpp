#include "test_sourcestatecontroller.h"
#include "test_globals.h"
#include "sourcestatecontroller.h"
#include "sourceswitchjson.h"
#include "sourcestateperiodicpollerfortest.h"
#include "sourcedeviceerrorinjection-forunittest.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"
#include <zera-json-params-state.h>

QTEST_MAIN(test_sourcestatecontroller)

void test_sourcestatecontroller::onIoQueueGroupFinished(IoQueueGroup::Ptr workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void test_sourcestatecontroller::init()
{
    TimerFactoryQtForTest::enableTest();
    m_sourceIo = nullptr;
    m_sourceIoWithError = nullptr;
    m_ioDevice = nullptr;
    m_listIoGroupsReceived.clear();

    m_ioDevice = createOpenDemoIoDevice();
    m_sourceIo = std::make_shared<SourceIoExtSerial>(m_ioDevice, DefaultTestSourceProperties());
    m_sourceIoWithError = std::make_shared<SourceIoErrorInjection>(m_sourceIo);
    connect(m_sourceIoWithError.get(), &AbstractSourceIo::sigResponseReceived,
            this, &test_sourcestatecontroller::onIoQueueGroupFinished);
}


void test_sourcestatecontroller::statePollAutoStart()
{
    setDemoIoFixedTimeout(m_ioDevice, 0);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    poller->setPollTime(0);
    int statePollSignalCount = 0;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] {
        statePollSignalCount++;
    });

    TimeMachineForTest::getInstance()->processTimers(2*shortQtEventTimeout);
    QVERIFY(statePollSignalCount > 0);
    QVERIFY(poller->isPeriodicPollActive());
}

void test_sourcestatecontroller::statePollChangeTime()
{
    setDemoIoFixedTimeout(m_ioDevice, 0);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    poller->setPollTime(shortQtEventTimeout);
    int statePollSignalCount = 0;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] {
        statePollSignalCount++;
    });

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout/2);
    QCOMPARE(statePollSignalCount, 0);
    TimeMachineForTest::getInstance()->processTimers(2*shortQtEventTimeout);
    QCOMPARE(statePollSignalCount, 1);
}

void test_sourcestatecontroller::stateInitialIdle()
{
    setDemoIoFixedTimeout(m_ioDevice, 0);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    poller->setPollTime(0);
    int statePollSignalCount = 0;
    SourceStateController::States stateReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        stateReceived = state;
        statePollSignalCount++;
    });

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(statePollSignalCount, 1);
    QCOMPARE(stateReceived, SourceStateController::States::IDLE);
}

void test_sourcestatecontroller::switchOnCausesBusyOnOffState()
{
    setDemoIoFixedTimeout(m_ioDevice, 0);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    poller->setPollTime(0);
    SourceSwitchJson switcher(m_sourceIo, notifyWrapperSwitch);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout); // ignore initial idle

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStateController::States::IDLE);
}

void test_sourcestatecontroller::switchOnOffCausesBusyTwoOnOffState()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    poller->setPollTime(0);
    SourceSwitchJson switcher(m_sourceIo, notifyWrapperSwitch);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout); // ignore initial idle

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);
    TimeMachineForTest::getInstance()->processTimers(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStateController::States::IDLE);

    switcher.switchOff();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(statesReceived.count(), 4);
    QCOMPARE(statesReceived[2], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[3], SourceStateController::States::IDLE);
}



void test_sourcestatecontroller::sequencePollSwitchErrorOnSwitch()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    QVERIFY(poller->tryStartPollNow());
    setDemoResonseErrorIdx(0);
    switcher.switchState(jsonParam);

    TimeMachineForTest::getInstance()->processTimers(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStateController::States::ERROR_SWITCH);
}

void test_sourcestatecontroller::sequencePollSwitchErrorOnPoll()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    setDemoResonseErrorIdx(0);
    QVERIFY(poller->tryStartPollNow());
    setDemoResonseErrorIdx(-1);
    switcher.switchState(jsonParam);

    TimeMachineForTest::getInstance()->processTimers(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStateController::States::ERROR_POLL);
}

void test_sourcestatecontroller::sequenceSwitchPollErrorOnSwitch()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    setDemoResonseErrorIdx(0);
    switcher.switchState(jsonParam);
    setDemoResonseErrorIdx(-1);
    QVERIFY(poller->tryStartPollNow());

    TimeMachineForTest::getInstance()->processTimers(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStateController::States::ERROR_SWITCH);
}

void test_sourcestatecontroller::sequenceSwitchPollErrorOnPoll()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    stateWatcher.setPollCountAfterSwitchOnOk(0);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    setDemoResonseErrorIdx(-1);
    switcher.switchState(jsonParam);
    setDemoResonseErrorIdx(0);
    poller->setPollTime(1);

    TimeMachineForTest::getInstance()->processTimers(100);
    QCOMPARE(statesReceived.count(), 3);
    QCOMPARE(statesReceived[0], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStateController::States::IDLE);
    QCOMPARE(statesReceived[2], SourceStateController::States::ERROR_POLL);
}

void test_sourcestatecontroller::sequenceSwitchPollErrorOnPostPoll()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    stateWatcher.setPollCountAfterSwitchOnOk(1);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    setDemoResonseErrorIdx(-1);
    switcher.switchState(jsonParam);
    setDemoResonseErrorIdx(0);
    poller->setPollTime(1);

    TimeMachineForTest::getInstance()->processTimers(100);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStateController::States::ERROR_SWITCH);
}

void test_sourcestatecontroller::sequencePollSwitchErrorOnBoth()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    setDemoResonseErrorIdx(0);
    QVERIFY(poller->tryStartPollNow());
    switcher.switchState(jsonParam);

    TimeMachineForTest::getInstance()->processTimers(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStateController::States::ERROR_POLL);
}

void test_sourcestatecontroller::sequenceSwitchPollErrorOnBoth()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState,poller);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    setDemoResonseErrorIdx(0);
    switcher.switchState(jsonParam);
    QVERIFY(poller->tryStartPollNow());

    TimeMachineForTest::getInstance()->processTimers(50);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0], SourceStateController::States::SWITCH_BUSY);
    QCOMPARE(statesReceived[1], SourceStateController::States::ERROR_SWITCH);
}

void test_sourcestatecontroller::pollStopsAfterSwitchError()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);

    setDemoResonseErrorIdx(0);
    switcher.switchState(jsonParam);
    poller->setPollTime(0);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[1], SourceStateController::States::ERROR_SWITCH);

    QVERIFY(!poller->isPeriodicPollActive());
}

void test_sourcestatecontroller::pollStopsAfterPollError()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    setDemoResonseErrorIdx(0);
    poller->setPollTime(0);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(statesReceived.count(), 1);
    QCOMPARE(statesReceived[0], SourceStateController::States::ERROR_POLL);

    QVERIFY(!poller->isPeriodicPollActive());
}

void test_sourcestatecontroller::pollStopsAfterErrorAndRestartsAfterSuccessfulSwitch()
{
    setDemoIoFixedTimeout(m_ioDevice, 1);

    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceTransactionStartNotifier::Ptr notifyWrapperState = SourceTransactionStartNotifier::Ptr::create(m_sourceIoWithError);
    SourceStatePeriodicPollerForTest::Ptr poller = SourceStatePeriodicPollerForTest::Ptr::create(notifyWrapperState);
    SourceStateController stateWatcher(notifyWrapperSwitch, notifyWrapperState, poller);
    SourceSwitchJson switcher(m_sourceIoWithError, notifyWrapperSwitch);

    QList<SourceStateController::States> statesReceived;
    connect(&stateWatcher, &SourceStateController::sigStateChanged, [&] (SourceStateController::States state) {
        statesReceived.append(state);
    });

    setDemoResonseErrorIdx(0);
    poller->setPollTime(0);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(statesReceived.count(), 1);
    QCOMPARE(statesReceived[0], SourceStateController::States::ERROR_POLL);

    QVERIFY(!poller->isPeriodicPollActive());

    setDemoResonseErrorIdx(-1);
    JsonParamApi jsonParam = switcher.getCurrLoadState();
    jsonParam.setOn(true);
    switcher.switchState(jsonParam);

    QVERIFY(poller->isPeriodicPollActive());
}

void test_sourcestatecontroller::setDemoResonseErrorIdx(int idx)
{
    static_cast<SourceIoErrorInjection*>(m_sourceIoWithError.get())->setDemoResonseErrorIdx(idx);
}


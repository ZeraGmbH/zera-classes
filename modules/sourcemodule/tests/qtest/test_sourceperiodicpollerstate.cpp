#include "test_sourceperiodicpollerstate.h"
#include "sourceioextserial.h"
#include "sourcestateperiodicpollerfortest.h"
#include "test_globals.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"

QTEST_MAIN(test_sourceperiodicpollerstate)

void test_sourceperiodicpollerstate::onIoQueueGroupFinished(IoQueueGroup::Ptr workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void test_sourceperiodicpollerstate::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_sourceperiodicpollerstate::init()
{
    m_sourceIo = nullptr;
    m_ioDevice = nullptr;
    m_transactionNotifier = nullptr;
    m_listIoGroupsReceived.clear();

    m_ioDevice = createOpenDemoIoDevice();
    m_sourceIo = std::make_shared<SourceIoExtSerial>(m_ioDevice, DefaultTestSourceProperties());
    m_transactionNotifier = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    connect(m_sourceIo.get(), &AbstractSourceIo::sigResponseReceived,
            this, &test_sourceperiodicpollerstate::onIoQueueGroupFinished);
}

void test_sourceperiodicpollerstate::pollAutoStart()
{
    SourceStatePeriodicPollerForTest poller(m_transactionNotifier);
    QVERIFY(poller.isPeriodicPollActive());
}

void test_sourceperiodicpollerstate::pollStop()
{
    SourceStatePeriodicPollerForTest poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(!poller.isPeriodicPollActive());
}

void test_sourceperiodicpollerstate::pollRestart()
{
    SourceStatePeriodicPollerForTest poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(!poller.isPeriodicPollActive());
    poller.startPeriodicPoll();
    QVERIFY(poller.isPeriodicPollActive());
}

void test_sourceperiodicpollerstate::pollImmediatePass()
{
    SourceStatePeriodicPollerForTest poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(poller.tryStartPollNow());
}

void test_sourceperiodicpollerstate::pollImmediateFail()
{
    SourceStatePeriodicPollerForTest poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(poller.tryStartPollNow());
    QVERIFY(!poller.tryStartPollNow());
}

void test_sourceperiodicpollerstate::pollAutoStartNotification()
{
    SourceStatePeriodicPollerForTest poller(m_transactionNotifier, 1);
    TimeMachineForTest::getInstance()->processTimers(30);
    QVERIFY(m_listIoGroupsReceived.count() > 0);
}

void test_sourceperiodicpollerstate::pollStopNotification()
{
    SourceStatePeriodicPollerForTest poller(m_transactionNotifier, 1);
    TimeMachineForTest::getInstance()->processTimers(30);
    m_listIoGroupsReceived.clear();
    poller.stopPeriodicPoll();
    QCOMPARE(m_listIoGroupsReceived.count(), 0);
}

void test_sourceperiodicpollerstate::pollChangeTimeNotification()
{
    SourceStatePeriodicPollerForTest poller(m_transactionNotifier, 500);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(m_listIoGroupsReceived.count(), 0);
    poller.setPollTime(10);
    TimeMachineForTest::getInstance()->processTimers(20);
    QCOMPARE(m_listIoGroupsReceived.count(), 2);
}

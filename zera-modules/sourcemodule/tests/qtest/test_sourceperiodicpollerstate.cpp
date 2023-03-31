#include "test_sourceperiodicpollerstate.h"
#include "test_globals.h"
#include "sourceperiodicpollerstate.h"

QTEST_MAIN(test_sourceperiodicpollerstate)

void test_sourceperiodicpollerstate::onIoQueueGroupFinished(IoQueueGroup::Ptr workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void test_sourceperiodicpollerstate::init()
{
    m_sourceIo = nullptr;
    m_ioDevice = nullptr;
    m_transactionNotifier = nullptr;
    m_listIoGroupsReceived.clear();

    m_ioDevice = createOpenDemoIoDevice();
    m_sourceIo = ISourceIo::Ptr(new SourceIo(m_ioDevice, DefaultTestSourceProperties()));
    m_transactionNotifier = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    connect(m_sourceIo.get(), &ISourceIo::sigResponseReceived,
            this, &test_sourceperiodicpollerstate::onIoQueueGroupFinished);
}

void test_sourceperiodicpollerstate::pollAutoStart()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    QVERIFY(poller.isPeriodicPollActive());
}

void test_sourceperiodicpollerstate::pollStop()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(!poller.isPeriodicPollActive());
}

void test_sourceperiodicpollerstate::pollRestart()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(!poller.isPeriodicPollActive());
    poller.startPeriodicPoll();
    QVERIFY(poller.isPeriodicPollActive());
}

void test_sourceperiodicpollerstate::pollImmediatePass()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(poller.tryStartPollNow());
}

void test_sourceperiodicpollerstate::pollImmediateFail()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(poller.tryStartPollNow());
    QVERIFY(!poller.tryStartPollNow());
}

void test_sourceperiodicpollerstate::pollAutoStartNotification()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier, 1);
    QTest::qWait(30);
    QVERIFY(m_listIoGroupsReceived.count() > 0);
}

void test_sourceperiodicpollerstate::pollStopNotification()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier, 1);
    QTest::qWait(30);
    m_listIoGroupsReceived.clear();
    poller.stopPeriodicPoll();
    QCOMPARE(m_listIoGroupsReceived.count(), 0);
}

void test_sourceperiodicpollerstate::pollChangeTimeNotification()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier, 500);
    QCoreApplication::processEvents();
    QCOMPARE(m_listIoGroupsReceived.count(), 0);
    poller.setPollTime(10);
    QTest::qWait(20);
    QCOMPARE(m_listIoGroupsReceived.count(), 1);
}

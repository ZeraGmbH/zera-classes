#include "main-unittest-qt.h"
#include "qt-unittest-sourceperiodicpollerstate.h"
#include "source-device/sourceperiodicpollerstate.h"

QTEST_MAIN(SourceStatePeriodicPollerTest);

void SourceStatePeriodicPollerTest::onIoQueueGroupFinished(IoQueueEntry::Ptr workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void SourceStatePeriodicPollerTest::init()
{
    m_sourceIo = nullptr;
    m_ioDevice = nullptr;
    m_transactionNotifier = nullptr;
    m_listIoGroupsReceived.clear();

    m_ioDevice = createOpenDemoIoDevice();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    m_sourceIo = ISourceIo::Ptr(new SourceIo(m_ioDevice, sourceProperties));
    m_transactionNotifier = SourceTransactionStartNotifier::Ptr::create(m_sourceIo);
    connect(m_sourceIo.get(), &ISourceIo::sigResponseReceived,
            this, &SourceStatePeriodicPollerTest::onIoQueueGroupFinished);
}

void SourceStatePeriodicPollerTest::pollAutoStart()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    QVERIFY(poller.isPeriodicPollActive());
}

void SourceStatePeriodicPollerTest::pollStop()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(!poller.isPeriodicPollActive());
}

void SourceStatePeriodicPollerTest::pollRestart()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(!poller.isPeriodicPollActive());
    poller.startPeriodicPoll();
    QVERIFY(poller.isPeriodicPollActive());
}

void SourceStatePeriodicPollerTest::pollImmediatePass()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(poller.tryStartPollNow());
}

void SourceStatePeriodicPollerTest::pollImmediateFail()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier);
    poller.stopPeriodicPoll();
    QVERIFY(poller.tryStartPollNow());
    QVERIFY(!poller.tryStartPollNow());
}

void SourceStatePeriodicPollerTest::pollAutoStartNotification()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier, 1);
    QTest::qWait(30);
    QVERIFY(m_listIoGroupsReceived.count() > 0);
}

void SourceStatePeriodicPollerTest::pollStopNotification()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier, 1);
    QTest::qWait(30);
    m_listIoGroupsReceived.clear();
    poller.stopPeriodicPoll();
    QCOMPARE(m_listIoGroupsReceived.count(), 0);
}

void SourceStatePeriodicPollerTest::pollChangeTimeNotification()
{
    SourceStatePeriodicPoller poller(m_transactionNotifier, 500);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_listIoGroupsReceived.count(), 0);
    poller.setPollTime(10);
    QTest::qWait(20);
    QCOMPARE(m_listIoGroupsReceived.count(), 1);
}

#include "test_taskrmchannelscheckavail.h"
#include "taskrmchannelscheckavail.h"
#include "rmtestanswers.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_taskrmchannelscheckavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmchannelscheckavail::init()
{
    m_rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    m_proxyClient = ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
}

void test_taskrmchannelscheckavail::okOnExpectedEqualGet()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, defaultResponse));
    QStringList expectedChannels = QString(defaultResponse).split(";");
    TaskCompositePtr task = TaskRmChannelsCheckAvail::create(m_rmInterface,
                                                             expectedChannels,
                                                             TIMEOUT_INFINITE);
    TaskTestHelperNew helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmchannelscheckavail::okOnExpectedPartOfGet()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, defaultResponse));
    QStringList expectedChannels = QString("m0;m1").split(";");
    TaskCompositePtr task = TaskRmChannelsCheckAvail::create(m_rmInterface,
                                                             expectedChannels,
                                                             TIMEOUT_INFINITE);
    TaskTestHelperNew helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmchannelscheckavail::errOnExpectedNotPartOfGet()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, defaultResponse));
    QStringList expectedChannels = QString("foo").split(";");
    TaskCompositePtr task = TaskRmChannelsCheckAvail::create(m_rmInterface,
                                                             expectedChannels,
                                                             TIMEOUT_INFINITE);
    TaskTestHelperNew helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskrmchannelscheckavail::timeoutAndErrFunc()
{
    int localErrorCount = 0;
    TaskCompositePtr task = TaskRmChannelsCheckAvail::create(m_rmInterface,
                                                             QStringList(),
                                                             DEFAULT_TIMEOUT,
                                                             [&]{
        localErrorCount++;
    });
    TaskTestHelperNew helper(task.get());
    task->start();
    QTest::qWait(DEFAULT_TIMEOUT_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

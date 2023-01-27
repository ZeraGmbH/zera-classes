#include "test_taskrmchannelscheckavail.h"
#include "taskrmchannelscheckavail.h"
#include "rminitfortest.h"
#include "tasktesthelper.h"
#include "scpifullcmdcheckerfortest.h"
#include <QTest>

QTEST_MAIN(test_taskrmchannelscheckavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmchannelscheckavail::okOnExpectedEqualGet()
{
    RmInitForTest rm;
    rm.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QStringList expectedChannels = QString(defaultResponse).split(";");
    TaskTemplatePtr task = TaskRmChannelsCheckAvail::create(rm.getRmInterface(),
                                                             expectedChannels,
                                                             EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmchannelscheckavail::okOnExpectedPartOfGet()
{
    RmInitForTest rm;
    rm.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QStringList expectedChannels = QString("m0;m1").split(";");
    TaskTemplatePtr task = TaskRmChannelsCheckAvail::create(rm.getRmInterface(),
                                                             expectedChannels,
                                                             EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmchannelscheckavail::errOnExpectedNotPartOfGet()
{
    RmInitForTest rm;
    rm.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QStringList expectedChannels = QString("foo").split(";");
    TaskTemplatePtr task = TaskRmChannelsCheckAvail::create(rm.getRmInterface(),
                                                             expectedChannels,
                                                             EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskrmchannelscheckavail::timeoutAndErrFunc()
{
    RmInitForTest rm;
    int localErrorCount = 0;
    TaskTemplatePtr task = TaskRmChannelsCheckAvail::create(rm.getRmInterface(),
                                                             QStringList(),
                                                             DEFAULT_EXPIRE,
                                                             [&]{
        localErrorCount++;
    });
    TaskTestHelper helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

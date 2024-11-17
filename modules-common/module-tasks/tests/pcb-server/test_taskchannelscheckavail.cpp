#include "test_taskchannelscheckavail.h"
#include "taskchannelscheckavail.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskrmchannelscheckavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmchannelscheckavail::okOnExpectedEqualGet()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QStringList expectedChannels = QString(defaultResponse).split(";");
    TaskTemplatePtr task = TaskChannelsCheckAvail::create(pcb.getPcbInterface(),
                                                            expectedChannels,
                                                            EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmchannelscheckavail::okOnExpectedPartOfGet()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QStringList expectedChannels = QString("m0;m1").split(";");
    TaskTemplatePtr task = TaskChannelsCheckAvail::create(pcb.getPcbInterface(),
                                                            expectedChannels,
                                                            EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmchannelscheckavail::errOnExpectedNotPartOfGet()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QStringList expectedChannels = QString("foo").split(";");
    TaskTemplatePtr task = TaskChannelsCheckAvail::create(pcb.getPcbInterface(),
                                                            expectedChannels,
                                                            EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskrmchannelscheckavail::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    TaskTemplatePtr task = TaskChannelsCheckAvail::create(pcb.getPcbInterface(),
                                                            QStringList(),
                                                            DEFAULT_EXPIRE,
                                                            [&]{
                                                                localErrorCount++;
                                                            });
    TaskTestHelper helper(task.get());
    task->start();
    TimeMachineForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

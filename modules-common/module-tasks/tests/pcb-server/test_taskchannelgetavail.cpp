#include "test_taskchannelgetavail.h"
#include "taskchannelgetavail.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskchannelgetavail)

static const char* defaultResponse = "m0;m1;m2;m6;m3;m4;m5;m7";

void test_taskchannelgetavail::checkScpiSend()
{
    PcbInitForTest pcb;
    QStringList channelList;
    TaskTemplatePtr task = TaskChannelGetAvail::create(pcb.getPcbInterface(),
                                                       channelList,
                                                       EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:CHANNEL:CAT");
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskchannelgetavail::returnsChannelListProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QStringList channelList;
    TaskTemplatePtr task = TaskChannelGetAvail::create(pcb.getPcbInterface(),
                                                       channelList,
                                                       EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList expectedRanges = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedRanges);
}

void test_taskchannelgetavail::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    QStringList channelList;
    TaskTemplatePtr task = TaskChannelGetAvail::create(pcb.getPcbInterface(),
                                                           channelList,
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

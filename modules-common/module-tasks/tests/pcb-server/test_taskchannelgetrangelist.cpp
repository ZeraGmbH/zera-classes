#include "test_taskchannelgetrangelist.h"
#include "taskchannelgetrangelist.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskchannelgetrangelist)

static const char* channelMName = "m0";
static const char* defaultResponse = "250V;8V;100mV";

void test_taskchannelgetrangelist::checkScpiSend()
{
    PcbInitForTest pcb;
    QStringList rangeList;
    TaskTemplatePtr task = TaskChannelGetRangeList::create(pcb.getPcbInterface(),
                                                           channelMName,
                                                           rangeList,
                                                           EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:RANGE:CATALOG").arg(channelMName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskchannelgetrangelist::returnsRangeListProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QStringList rangeList;
    TaskTemplatePtr task = TaskChannelGetRangeList::create(pcb.getPcbInterface(),
                                                           channelMName,
                                                           rangeList,
                                                           EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList expectedRanges = QString(defaultResponse).split(";");
    QCOMPARE(rangeList, expectedRanges);
}

void test_taskchannelgetrangelist::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    QStringList rangeList;
    TaskTemplatePtr task = TaskChannelGetRangeList::create(pcb.getPcbInterface(),
                                                           channelMName,
                                                           rangeList,
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

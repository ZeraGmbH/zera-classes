#include "test_taskchannelgetcurrentrange.h"
#include "taskchannelgetcurrentrange.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskchannelgetcurrentrange)

static const char* channelName = "m0";
static const char* defaultResponse = "250V";

void test_taskchannelgetcurrentrange::checkScpiSend()
{
    PcbInitForTest pcb;
    QString currentRange;
    TaskTemplatePtr task = TaskChannelGetCurrentRange::create(pcb.getPcbInterface(),
                                                              channelName,
                                                              currentRange,
                                                              EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:RANGE").arg(channelName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskchannelgetcurrentrange::returnsCurrentRangeProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, QString(defaultResponse)));
    QString currentRange;
    TaskTemplatePtr task = TaskChannelGetCurrentRange::create(pcb.getPcbInterface(),
                                                       channelName,
                                                       currentRange,
                                                       EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(currentRange, defaultResponse);
}

void test_taskchannelgetcurrentrange::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    QString currentRange;
    TaskTemplatePtr task = TaskChannelGetCurrentRange::create(pcb.getPcbInterface(),
                                                              channelName,
                                                              currentRange,
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

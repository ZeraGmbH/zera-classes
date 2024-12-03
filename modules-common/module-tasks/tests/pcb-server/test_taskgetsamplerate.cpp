#include "test_taskgetsamplerate.h"
#include "taskgetsamplerate.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskgetsamplerate)

void test_taskgetsamplerate::checkScpiSend()
{
    PcbInitForTest pcb;
    int samplingRate;
    TaskTemplatePtr task = TaskGetSampleRate::create(pcb.getPcbInterface(),
                                                       samplingRate,
                                                       EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SAMPLE:SRATE");
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));

}

void test_taskgetsamplerate::returnsSampleRateProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, "504"));
    int sampleRate;
    TaskTemplatePtr task = TaskGetSampleRate::create(pcb.getPcbInterface(),
                                                       sampleRate,
                                                       EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(sampleRate, 504);
}

void test_taskgetsamplerate::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    int sampleRate;
    TaskTemplatePtr task = TaskGetSampleRate::create(pcb.getPcbInterface(),
                                                       sampleRate,
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

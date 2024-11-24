#include "test_taskrangegetisavailable.h"
#include "taskrangegetisavailable.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskrangegetisavailable)

static const char* channelSysName = "m0";
static const char* rangeName = "250V";
static const bool defaultAvailValue = true;

void test_taskrangegetisavailable::checkScpiSend()
{
    PcbInitForTest pcb;
    bool isAvailable;
    TaskTemplatePtr task = TaskRangeGetIsAvailable::create(pcb.getPcbInterface(),
                                                           channelSysName, rangeName,
                                                           isAvailable,
                                                           EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:%2:AVAIL").arg(channelSysName, rangeName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskrangegetisavailable::returnsIsAvailProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, QString("%1").arg(defaultAvailValue)));
    bool isAvailable = !defaultAvailValue;
    TaskTemplatePtr task = TaskRangeGetIsAvailable::create(pcb.getPcbInterface(),
                                                           channelSysName, rangeName,
                                                           isAvailable,
                                                           EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(isAvailable, defaultAvailValue);
}

void test_taskrangegetisavailable::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    bool isAvailable = !defaultAvailValue;
    TaskTemplatePtr task = TaskRangeGetIsAvailable::create(pcb.getPcbInterface(),
                                                           channelSysName, rangeName,
                                                           isAvailable,
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

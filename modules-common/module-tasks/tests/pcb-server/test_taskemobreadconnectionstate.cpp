#include "test_taskemobreadconnectionstate.h"
#include "taskemobreadconnectionstate.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskemobreadconnectionstate)

void test_taskemobreadconnectionstate::checkScpiSend()
{
    PcbInitForTest pcb;
    std::shared_ptr<int> connState = std::make_shared<int>();
    TaskTemplatePtr task = TaskEmobReadConnectionState::create(pcb.getPcbInterface(),
                                                               connState, "",
                                                               EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SYST:EMOB:LOCKST");
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskemobreadconnectionstate::returnsConnectionStateProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, "4"));
    std::shared_ptr<int> connState = std::make_shared<int>();
    TaskTemplatePtr task = TaskEmobReadConnectionState::create(pcb.getPcbInterface(),
                                                               connState, "",
                                                               EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*connState, 4);
}

void test_taskemobreadconnectionstate::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<int> connState = std::make_shared<int>();
    TaskTemplatePtr task = TaskEmobReadConnectionState::create(pcb.getPcbInterface(),
                                                               connState, "",
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

#include "test_taskrangegeturvalue.h"
#include "taskrangegeturvalue.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskrangegeturvalue)

static const char* channelMName = "m0";
static const char* rangeName = "250V";
static double defaultUrValue = 123456.0; // although treated as double - it is more an int...

void test_taskrangegeturvalue::checkScpiSend()
{
    PcbInitForTest pcb;
    std::shared_ptr<double> urValue = std::make_shared<double>();
    TaskTemplatePtr task = TaskRangeGetUrValue::create(pcb.getPcbInterface(),
                                                       channelMName, rangeName,
                                                       urValue,
                                                       EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:%2:URVALUE").arg(channelMName, rangeName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskrangegeturvalue::returnsUrValueProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, QString("%1").arg(defaultUrValue)));
    std::shared_ptr<double> urValue = std::make_shared<double>(0.0);
    TaskTemplatePtr task = TaskRangeGetUrValue::create(pcb.getPcbInterface(),
                                                       channelMName, rangeName,
                                                       urValue,
                                                       EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*urValue, defaultUrValue);
}

void test_taskrangegeturvalue::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<double> urValue = std::make_shared<double>();
    TaskTemplatePtr task = TaskRangeGetUrValue::create(pcb.getPcbInterface(),
                                                       channelMName, rangeName,
                                                       urValue,
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

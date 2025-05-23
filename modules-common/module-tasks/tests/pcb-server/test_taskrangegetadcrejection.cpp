#include "test_taskrangegetadcrejection.h"
#include "taskrangegetadcrejection.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskrangegetadcrejection)

static const char* channelMName = "m0";
static const char* rangeName = "250V";
static double defaultRejection = 123456.0; // although treated as double - it is more an int...

void test_taskrangegetadcrejection::checkScpiSend()
{
    PcbInitForTest pcb;
    std::shared_ptr<double> rejection = std::make_shared<double>();
    TaskTemplatePtr task = TaskRangeGetAdcRejection::create(pcb.getPcbInterface(),
                                                            channelMName, rangeName,
                                                            rejection,
                                                            EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:%2:ADCREJECTION").arg(channelMName, rangeName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskrangegetadcrejection::returnsRejectionProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, QString("%1").arg(defaultRejection)));
    std::shared_ptr<double> rejection = std::make_shared<double>(0.0);
    TaskTemplatePtr task = TaskRangeGetAdcRejection::create(pcb.getPcbInterface(),
                                                            channelMName, rangeName,
                                                            rejection,
                                                            EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*rejection, defaultRejection);
}

void test_taskrangegetadcrejection::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<double> rejection = std::make_shared<double>(0.0);
    TaskTemplatePtr task = TaskRangeGetAdcRejection::create(pcb.getPcbInterface(),
                                                            channelMName, rangeName,
                                                            rejection,
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

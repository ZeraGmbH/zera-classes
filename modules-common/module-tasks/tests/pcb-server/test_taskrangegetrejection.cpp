#include "test_taskrangegetrejection.h"
#include "taskrangegetrejection.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskrangegetrejection)

static const char* channelMName = "m0";
static const char* rangeName = "250V";
static double defaultRejection = 123456.0; // although treated as double - it is more an int...

void test_taskrangegetrejection::checkScpiSend()
{
    PcbInitForTest pcb;
    std::shared_ptr<double> rejection = std::make_shared<double>();
    TaskTemplatePtr task = TaskRangeGetRejection::create(pcb.getPcbInterface(),
                                                            channelMName, rangeName,
                                                            rejection,
                                                            EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:%2:REJECTION").arg(channelMName, rangeName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskrangegetrejection::returnsRejectionProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, QString("%1").arg(defaultRejection)));
    std::shared_ptr<double> rejection = std::make_shared<double>(0.0);
    TaskTemplatePtr task = TaskRangeGetRejection::create(pcb.getPcbInterface(),
                                                            channelMName, rangeName,
                                                            rejection,
                                                            EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*rejection, defaultRejection);
}

void test_taskrangegetrejection::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<double> rejection = std::make_shared<double>(0.0);
    TaskTemplatePtr task = TaskRangeGetRejection::create(pcb.getPcbInterface(),
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

#include "test_taskchannelgetrejection.h"
#include "taskchannelgetrejection.h"
#include "pcbinitfortest.h"
#include <QTest>

QTEST_MAIN(test_taskchannelgetrejection)

static const char* channelSysName = "m0";
static const char* rangeName = "250V";
static double defaultRecection = 123456.0; // although treated as double - it is more an int...

void test_taskchannelgetrejection::checkScpiSend()
{
    PcbInitForTest pcb;
    double rejection;
    TaskCompositePtr task = TaskChannelGetRejection::create(pcb.getPcbInterface(),
                                                            channelSysName, rangeName,
                                                            rejection,
                                                            EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:%2:REJECTION").arg(channelSysName, rangeName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskchannelgetrejection::returnsOvrRejectionProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, QString("%1").arg(defaultRecection)));
    double rejection = 0.0;
    TaskCompositePtr task = TaskChannelGetRejection::create(pcb.getPcbInterface(),
                                                            channelSysName, rangeName,
                                                            rejection,
                                                            EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(rejection, defaultRecection);
}

void test_taskchannelgetrejection::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    double rejection = 0.0;
    TaskCompositePtr task = TaskChannelGetRejection::create(pcb.getPcbInterface(),
                                                            channelSysName, rangeName,
                                                            rejection,
                                                            DEFAULT_EXPIRE,
                                                            [&]{
        localErrorCount++;
    });
    TaskTestHelper helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

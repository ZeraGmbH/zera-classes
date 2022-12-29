#include "test_taskchannelgetovrejection.h"
#include "taskchannelgetovrejection.h"
#include "pcbinitfortest.h"
#include <QTest>

QTEST_MAIN(test_taskchannelgetovrejection)

static const char* channelSysName = "m0";
static const char* rangeName = "250V";
static double defaultOvrRecection = 123456.0; // although treated as double - it is more an int...

void test_taskchannelgetovrejection::checkScpiSend()
{
    PcbInitForTest pcb;
    double ovrRecection;
    TaskCompositePtr task = TaskChannelGetOvRejection::create(pcb.getPcbInterface(),
                                                              channelSysName, rangeName,
                                                              ovrRecection,
                                                              EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:%2:OVREJECTION").arg(channelSysName, rangeName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskchannelgetovrejection::returnsOvrRejectionProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, QString("%1").arg(defaultOvrRecection)));
    double ovrRecection = 0.0;
    TaskCompositePtr task = TaskChannelGetOvRejection::create(pcb.getPcbInterface(),
                                                              channelSysName, rangeName,
                                                              ovrRecection,
                                                              EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(ovrRecection, defaultOvrRecection);
}

void test_taskchannelgetovrejection::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    double ovrRecection = 0.0;
    TaskCompositePtr task = TaskChannelGetOvRejection::create(pcb.getPcbInterface(),
                                                              channelSysName, rangeName,
                                                              ovrRecection,
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

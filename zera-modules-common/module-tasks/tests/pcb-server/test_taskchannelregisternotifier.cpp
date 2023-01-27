#include "test_taskchannelregisternotifier.h"
#include "taskchannelregisternotifier.h"
#include "pcbinitfortest.h"
#include "tasktesthelper.h"
#include "scpifullcmdcheckerfortest.h"
#include <QTest>

QTEST_MAIN(test_taskchannelregisternotifier)

static const char* channelSysName = "m0";

void test_taskchannelregisternotifier::checkScpiSend()
{
    PcbInitForTest pcb;
    double urValue;
    TaskTemplatePtr task = TaskChannelRegisterNotifier::create(pcb.getPcbInterface(),
                                                                channelSysName,
                                                                EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SERVER:REGISTER");
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isCmdwP, 2);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskchannelregisternotifier::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    double urValue;
    TaskTemplatePtr task = TaskChannelRegisterNotifier::create(pcb.getPcbInterface(),
                                                                channelSysName,
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

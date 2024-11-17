#include "test_taskregisternotifier.h"
#include "taskregisternotifier.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskregisternotifier)

static const char* scpiQuery = "FOO:BAR?";
static const int notificationId = 42;

void test_taskregisternotifier::checkScpiSend()
{
    PcbInitForTest pcb;
    TaskTemplatePtr task = TaskRegisterNotifier::create(pcb.getPcbInterface(),
                                                        scpiQuery,
                                                        notificationId,
                                                        EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SERVER:REGISTER");
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isCmdwP, 2);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskregisternotifier::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    TaskTemplatePtr task = TaskRegisterNotifier::create(pcb.getPcbInterface(),
                                                        scpiQuery,
                                                        notificationId,
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

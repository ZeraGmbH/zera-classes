#include "test_taskunregisternotifier.h"
#include "taskunregisternotifier.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskunregisternotifier)

void test_taskunregisternotifier::checkScpiSend()
{
    PcbInitForTest pcb;
    TaskTemplatePtr task = TaskUnregisterNotifier::create(pcb.getPcbInterface(),
                                                          EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SERVER:UNREGISTER");
    // TODO (checked) server side does not expect params!!!
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isCmdwP, 1);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskunregisternotifier::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    TaskTemplatePtr task = TaskUnregisterNotifier::create(pcb.getPcbInterface(),
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

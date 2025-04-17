#include "test_taskremovecmdlistsforallclients.h"
#include "taskremovecmdlistsforallclients.h"
#include "scpifullcmdcheckerfortest.h"
#include <dspinitfortest.h>
#include <tasktesthelper.h>
#include <timertestdefaults.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_taskremovecmdlistsforallclients)

void test_taskremovecmdlistsforallclients::checkScpiSend()
{
    DspInitForTest dsp;
    TaskTemplatePtr task = TaskRemoveCmdListsForAllClients::create(dsp.getDspInterface(), EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = dsp.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    ScpiFullCmdCheckerForTest scpiChecker("MEASURE:LIST:CLALL", SCPI::isCmd);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskremovecmdlistsforallclients::timeoutAndErrFunc()
{
    DspInitForTest dsp;
    int localErrorCount = 0;
    QString channelAlias;
    TaskTemplatePtr task = TaskRemoveCmdListsForAllClients::create(dsp.getDspInterface(),
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

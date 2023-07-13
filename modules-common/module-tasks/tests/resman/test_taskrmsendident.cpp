#include "test_taskrmsendident.h"
#include "taskrmsendident.h"
#include "rminitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskrmsendident)

static const char* testIdent = "foo";

void test_taskrmsendident::checkSend()
{
    RmInitForTest rm;
    TaskTemplatePtr task = TaskRmSendIdent::create(rm.getRmInterface(),
                                                   testIdent,
                                                   EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(rm.getProxyClient()->getReceivedCommands().count(), 0);
    QStringList identsSent = rm.getProxyClient()->getReceivedIdents();
    QCOMPARE(identsSent.count(), 1);
    QCOMPARE(identsSent[0], testIdent);
}

void test_taskrmsendident::timeoutAndErrFunc()
{
    RmInitForTest rm;
    int localErrorCount = 0;
    QStringList channelList;
    TaskTemplatePtr task = TaskRmSendIdent::create(rm.getRmInterface(),
                                                   testIdent,
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

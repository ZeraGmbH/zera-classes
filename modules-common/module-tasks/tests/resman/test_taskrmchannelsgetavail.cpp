#include "test_taskrmchannelsgetavail.h"
#include "taskrmchannelsgetavail.h"
#include "rminitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskrmchannelsgetavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmchannelsgetavail::checkScpiSend()
{
    RmInitForTest rm;
    QStringList channelList;
    TaskTemplatePtr task = TaskRmChannelsGetAvail::create(rm.getRmInterface(),
                                                          EXPIRE_INFINITE,
                                                          channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList scpiSent = rm.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    ScpiFullCmdCheckerForTest scpiChecker("RESOURCE:SENSE:CAT", SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskrmchannelsgetavail::getThreeChannels()
{
    RmInitForTest rm;
    rm.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QStringList channelList;
    TaskTemplatePtr task = TaskRmChannelsGetAvail::create(rm.getRmInterface(),
                                                          EXPIRE_INFINITE,
                                                          channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

void test_taskrmchannelsgetavail::getThreeChannelsIgnoreMMode()
{
    RmInitForTest rm;
    rm.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, "m0;m1;m2;MMODE"));
    QStringList channelList;
    TaskTemplatePtr task = TaskRmChannelsGetAvail::create(rm.getRmInterface(),
                                                          EXPIRE_INFINITE,
                                                          channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

void test_taskrmchannelsgetavail::timeoutAndErrFunc()
{
    RmInitForTest rm;
    int localErrorCount = 0;
    QStringList channelList;
    TaskTemplatePtr task = TaskRmChannelsGetAvail::create(rm.getRmInterface(),
                                                          DEFAULT_EXPIRE,
                                                          channelList,
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

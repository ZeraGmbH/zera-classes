#include "test_taskrmchannelsgetavail.h"
#include "taskrmchannelsgetavail.h"
#include "rmtestanswers.h"
#include <tasktesthelper.h>
#include <scpifullcmdcheckerfortest.h>
#include <timerrunnerfortest.h>
#include <zeratimerfactorymethodstest.h>
#include <QTest>

QTEST_MAIN(test_taskrmchannelsgetavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmchannelsgetavail::init()
{
    m_rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    m_proxyClient = ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
    TimerRunnerForTest::reset();
    ZeraTimerFactoryMethodsTest::enableTest();
}

void test_taskrmchannelsgetavail::checkScpiSend()
{
    QStringList channelList;
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(m_rmInterface,
                                                           EXPIRE_INFINITE,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList scpiSent = m_proxyClient->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    ScpiFullCmdCheckerForTest scpiChecker("RESOURCE:SENSE:CAT", SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskrmchannelsgetavail::getThreeChannels()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, defaultResponse));
    QStringList channelList;
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(m_rmInterface,
                                                           EXPIRE_INFINITE,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

void test_taskrmchannelsgetavail::getThreeChannelsIgnoreMMode()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, "m0;m1;m2;MMODE"));
    QStringList channelList;
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(m_rmInterface,
                                                           EXPIRE_INFINITE,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

void test_taskrmchannelsgetavail::timeoutAndErrFunc()
{
    int localErrorCount = 0;
    QStringList channelList;
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(m_rmInterface,
                                                           DEFAULT_EXPIRE,
                                                           channelList,
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

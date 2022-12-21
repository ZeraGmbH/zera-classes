#include "test_taskrmchannelsgetavail.h"
#include "taskrmchannelsgetavail.h"
#include "rmtestanswers.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_taskrmchannelsgetavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmchannelsgetavail::init()
{
    m_rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    m_proxyClient = ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
}

void test_taskrmchannelsgetavail::getThreeChannels()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, defaultResponse));
    QStringList channelList;
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(m_rmInterface,
                                                           TIMEOUT_INFINITE,
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
                                                           TIMEOUT_INFINITE,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

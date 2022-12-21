#include "test_taskrmchannelsgetavail.h"
#include "taskrmchannelsgetavail.h"
#include "rminterface.h"
#include "rmtestanswers.h"
#include "tasktesthelper.h"
#include "proxyclientfortest.h"
#include <QTest>

QTEST_MAIN(test_taskrmchannelsgetavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmchannelsgetavail::getThreeChannels()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    proxyClient->setAnswers(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

void test_taskrmchannelsgetavail::getThreeChannelsIgnoreMMode()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "m0;m1;m2;MMODE");
    proxyClient->setAnswers(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}


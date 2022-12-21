#include "test_taskrmchannelscheckavail.h"
#include "taskrmchannelscheckavail.h"
#include "rminterface.h"
#include "rmtestanswers.h"
#include "tasktesthelper.h"
#include "proxyclientfortest.h"
#include <QTest>

QTEST_MAIN(test_taskrmchannelscheckavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmchannelscheckavail::okOnExpectedEqualGet()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    Zera::Proxy::ProxyClientPtr proxyClient = ProxyClientForTest::create(answers);
    rmInterface->setClientSmart(proxyClient);

    QStringList expectedChannels = QString(defaultResponse).split(";");
    TaskCompositePtr task = TaskRmChannelsCheckAvail::create(rmInterface,
                                                             expectedChannels,
                                                             DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmchannelscheckavail::okOnExpectedPartOfGet()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    Zera::Proxy::ProxyClientPtr proxyClient = ProxyClientForTest::create(answers);
    rmInterface->setClientSmart(proxyClient);

    QStringList expectedChannels = QString("m0;m1").split(";");
    TaskCompositePtr task = TaskRmChannelsCheckAvail::create(rmInterface,
                                                             expectedChannels,
                                                             DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmchannelscheckavail::errOnExpectedNotPartOfGet()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    Zera::Proxy::ProxyClientPtr proxyClient = ProxyClientForTest::create(answers);
    rmInterface->setClientSmart(proxyClient);

    QStringList expectedChannels = QString("foo").split(";");
    TaskCompositePtr task = TaskRmChannelsCheckAvail::create(rmInterface,
                                                             expectedChannels,
                                                             DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}


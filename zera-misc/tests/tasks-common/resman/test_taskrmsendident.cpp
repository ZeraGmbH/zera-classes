#include "test_taskrmsendident.h"
#include "taskrmsendident.h"
#include "rmtestanswers.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_taskrmsendident)

static const char* testIdent = "foo";

void test_taskrmsendident::init()
{
    m_rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    m_proxyClient = ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
}

void test_taskrmsendident::checkSend()
{
    TaskCompositePtr task = TaskRmSendIdent::create(m_rmInterface,
                                                    testIdent,
                                                    TIMEOUT_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(m_proxyClient->getReceivedCommands().count(), 0);
    QStringList identsSent = m_proxyClient->getReceivedIdents();
    QCOMPARE(identsSent.count(), 1);
    QCOMPARE(identsSent[0], testIdent);
}

void test_taskrmsendident::timeoutAndErrFunc()
{
    int localErrorCount = 0;
    QStringList channelList;
    TaskCompositePtr task = TaskRmSendIdent::create(m_rmInterface,
                                                    testIdent,
                                                    DEFAULT_TIMEOUT,
                                                    [&]{
        localErrorCount++;
    });
    TaskTestHelperNew helper(task.get());
    task->start();
    QTest::qWait(DEFAULT_TIMEOUT_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

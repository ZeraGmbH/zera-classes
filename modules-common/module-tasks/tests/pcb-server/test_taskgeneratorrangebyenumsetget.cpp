#include "test_taskgeneratorrangebyenumsetget.h"
#include "taskgeneratorrangebyenumset.h"
#include "taskgeneratorrangebyenumget.h"
#include <pcbinitfortest.h>
#include <testfactoryi2cctrl.h>
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <mocktcpnetworkfactory.h>
#include <resmanrunfacade.h>
#include <mockmt310s2d.h>
#include <proxyclient.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_taskgeneratorrangebyenumsetget)

void test_taskgeneratorrangebyenumsetget::checkScpiSendReceive()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    std::unique_ptr<ResmanRunFacade> resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    std::unique_ptr<MockMt310s2d> mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory, "mt581s2d");
    TimeMachineObject::feedEventLoop();

    Zera::ProxyClientPtr proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    std::shared_ptr<Zera::cPCBInterface> pcbIFace = std::make_shared<Zera::cPCBInterface>();
    pcbIFace->setClientSmart(proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(proxyClient);
    TimeMachineObject::feedEventLoop();

    TaskTemplatePtr task = TaskGeneratorRangeByEnumSet::create(pcbIFace,
                                                         "m0", 42,
                                                         []{}, EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);

    std::shared_ptr<int> rangeReceived = std::make_shared<int>();
    task = TaskGeneratorRangeByEnumGet::create(pcbIFace,
                                               "m0", rangeReceived,
                                               []{}, EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*rangeReceived, 42);
}

void test_taskgeneratorrangebyenumsetget::returnsNak()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(nack, ""));
    TaskTemplatePtr task = TaskGeneratorRangeByEnumSet::create(pcb.getPcbInterface(),
                                                         "m0", 2,
                                                         []{}, EXPIRE_INFINITE);
    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], false);
}

void test_taskgeneratorrangebyenumsetget::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    TaskTemplatePtr task = TaskGeneratorRangeByEnumSet::create(pcb.getPcbInterface(),
                                                         "m0", 2,
                                                         [&]{ localErrorCount++; }, DEFAULT_EXPIRE);
    TaskTestHelper helper(task.get());
    task->start();
    TimeMachineForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

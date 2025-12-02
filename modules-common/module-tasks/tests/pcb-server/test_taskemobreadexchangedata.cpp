#include "test_taskemobreadexchangedata.h"
#include "taskemobreadexchangedata.h"
#include "pcbinitfortest.h"
#include <controllerpersitentdata.h>
#include <mocktcpnetworkfactory.h>
#include <mocki2cctrlemob.h>
#include <testfactoryi2cctrl.h>
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_taskemobreadexchangedata)

void test_taskemobreadexchangedata::cleanup()
{
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
}

void test_taskemobreadexchangedata::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<QByteArray> dataReceived = std::make_shared<QByteArray>();
    TaskTemplatePtr task = TaskEmobReadExchangeData::create(pcb.getPcbInterface(),
                                                            dataReceived, "",
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

void test_taskemobreadexchangedata::readProperly()
{
    setupServers();
    const QString channelAlias = "IL1";
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert(channelAlias, {"EMOB_MOCK-00V00", cClamp::undefined});
    m_mt310s2d->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    std::shared_ptr<QByteArray> dataReceived = std::make_shared<QByteArray>();
    TaskTemplatePtr task = TaskEmobReadExchangeData::create(m_pcbIFace,
                                                            dataReceived, "m3",
                                                            EXPIRE_INFINITE);
    QSignalSpy spy(task.get(), &TaskEmobReadExchangeData::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(*dataReceived, MockI2cCtrlEMOB::getDefaultExchangeData());
    QCOMPARE(spy[0][0], true);
}

void test_taskemobreadexchangedata::readWrongChannel()
{
    setupServers();
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB_MOCK-00V00", cClamp::undefined});
    m_mt310s2d->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    std::shared_ptr<QByteArray> dataReceived = std::make_shared<QByteArray>();
    TaskTemplatePtr task = TaskEmobReadExchangeData::create(m_pcbIFace,
                                                            dataReceived, "m4",
                                                            EXPIRE_INFINITE);
    QSignalSpy spy(task.get(), &TaskEmobReadExchangeData::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy[0][0], false);
}

void test_taskemobreadexchangedata::readNoEmob()
{
    setupServers();

    std::shared_ptr<QByteArray> dataReceived = std::make_shared<QByteArray>();
    TaskTemplatePtr task = TaskEmobReadExchangeData::create(m_pcbIFace,
                                                            dataReceived, "m3",
                                                            EXPIRE_INFINITE);
    QSignalSpy spy(task.get(), &TaskEmobReadExchangeData::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], false);
}

void test_taskemobreadexchangedata::setupServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory, "mt310s2d");
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_shared<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

#include "test_taskemobwriteexchangedata.h"
#include "taskemobwriteexchangedata.h"
#include "pcbinitfortest.h"
#include <controllerpersitentdata.h>
#include <mocktcpnetworkfactory.h>
#include <testfactoryi2cctrl.h>
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_taskemobwriteexchangedata)

void test_taskemobwriteexchangedata::cleanup()
{
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
}

void test_taskemobwriteexchangedata::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    TaskTemplatePtr task = TaskEmobWriteExchangeData::create(pcb.getPcbInterface(),
                                                             "", 0, QByteArray(),
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

void test_taskemobwriteexchangedata::writeProperly()
{
    setupServers();
    const QString channelAlias = "IL1";
    constexpr int emobId = 2;
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert(channelAlias, {"EMOB_MOCK-00V00", cClamp::undefined});
    m_mt310s2d->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QByteArray testData = createTestEmobExchangeData();
    TaskTemplatePtr task = TaskEmobWriteExchangeData::create(m_pcbIFace,
                                                             "m3", emobId, testData,
                                                             EXPIRE_INFINITE);
    QSignalSpy spy(task.get(), &TaskEmobWriteExchangeData::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    int muxChannel = m_mt310s2d->getSenseSettings()->findChannelSettingByAlias1(channelAlias)->m_nMuxChannelNo;
    QCOMPARE(testData, ControllerPersitentData::getData().m_hotpluggedDevices[muxChannel].emobDataReceived[emobId]);
    QCOMPARE(spy[0][0], true);
}

void test_taskemobwriteexchangedata::writeProperlyEmpty()
{
    setupServers();
    const QString channelAlias = "IL1";
    constexpr int emobId = 2;
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert(channelAlias, {"EMOB_MOCK-00V00", cClamp::undefined});
    m_mt310s2d->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QByteArray testData = QByteArray();
    TaskTemplatePtr task = TaskEmobWriteExchangeData::create(m_pcbIFace,
                                                             "m3", emobId, testData,
                                                             EXPIRE_INFINITE);
    QSignalSpy spy(task.get(), &TaskEmobWriteExchangeData::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    int muxChannel = m_mt310s2d->getSenseSettings()->findChannelSettingByAlias1(channelAlias)->m_nMuxChannelNo;
    QByteArray found = ControllerPersitentData::getData().m_hotpluggedDevices[muxChannel].emobDataReceived[emobId];
    QCOMPARE(testData, found);
    QCOMPARE(spy[0][0], true);
}

void test_taskemobwriteexchangedata::writeWrongChannel()
{
    setupServers();
    const QString channelAlias = "IL1";
    constexpr int emobId = 2;
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert(channelAlias, {"EMOB_MOCK-00V00", cClamp::undefined});
    m_mt310s2d->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QByteArray testData = createTestEmobExchangeData();
    TaskTemplatePtr task = TaskEmobWriteExchangeData::create(m_pcbIFace,
                                                             "m5", emobId, testData,
                                                             EXPIRE_INFINITE);
    QSignalSpy spy(task.get(), &TaskEmobWriteExchangeData::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy[0][0], false);
}

void test_taskemobwriteexchangedata::writeNoEmob()
{
    setupServers();

    constexpr int emobId = 2;
    QByteArray testData = createTestEmobExchangeData();
    TaskTemplatePtr task = TaskEmobWriteExchangeData::create(m_pcbIFace,
                                                             "m3", emobId, testData,
                                                             EXPIRE_INFINITE);
    QSignalSpy spy(task.get(), &TaskEmobWriteExchangeData::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], false);
}

void test_taskemobwriteexchangedata::setupServers()
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

QByteArray test_taskemobwriteexchangedata::createTestEmobExchangeData()
{
    QByteArray emobExchangeData;
    emobExchangeData.resize(256);
    for(int i=0; i<emobExchangeData.size(); ++i)
        emobExchangeData[i] = i;
    return emobExchangeData;
}

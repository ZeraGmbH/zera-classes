#include "test_pcb_channel_manager.h"
#include "pcbchannelmanager.h"
#include <timemachineobject.h>
#include <testallservicesmt310s2.h>
#include <mocktcpnetworkfactory.h>
#include <proxy.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QTest>
#include <QSignalSpy>

QTEST_MAIN(test_pcb_channel_manager)

void test_pcb_channel_manager::initTestCase()
{
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_i2cFactory = std::make_shared<TestFactoryI2cCtrl>(false);
    TimerFactoryQtForTest::enableTest();
}

void test_pcb_channel_manager::init()
{
    TimeMachineForTest::reset();
}

void test_pcb_channel_manager::emptyChannelListOnStartup()
{
    PcbChannelManager manager;
    QVERIFY(manager.getChannelMNames().isEmpty());
}

void test_pcb_channel_manager::scanChannelListSignalReturned()
{
    std::unique_ptr<AbstractMockAllServices> allServices =
        std::make_unique<TestAllServicesMt310s2>(m_tcpFactory, m_i2cFactory);
    Zera::ProxyClientPtr client = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpFactory);

    PcbChannelManager manager;
    QSignalSpy spy(&manager, &PcbChannelManager::sigScanFinished);

    manager.startScan(client);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_pcb_channel_manager::scanChannelListChannelsReturned()
{
    std::unique_ptr<AbstractMockAllServices> allServices =
        std::make_unique<TestAllServicesMt310s2>(m_tcpFactory, m_i2cFactory);
    Zera::ProxyClientPtr client = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpFactory);

    PcbChannelManager manager;
    manager.startScan(client);
    TimeMachineObject::feedEventLoop();

    QStringList channels = manager.getChannelMNames();
    QCOMPARE(channels.count(), 8);
}

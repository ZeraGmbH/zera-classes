#include "test_cro_range.h"
#include "range.h"
#include <networkconnectioninfo.h>
#include <clampfactorytest.h>
#include <testfactoryi2cctrl.h>
#include <mocki2ceepromiofactory.h>
#include <mocktcpnetworkfactory.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <proxy.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_cro_range)

static const NetworkConnectionInfo netInfo("127.0.0.1", 6307);

using namespace ChannelRangeObserver;

void test_cro_range::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_cro_range::init()
{
    setupServers();
    setupClient();
}

void test_cro_range::cleanup()
{
    m_pcbInterface = nullptr;
    m_pcbClient = nullptr;
    TimeMachineObject::feedEventLoop();
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_cro_range::defaultOnStartup()
{
    Range range("m0", "250V", netInfo, m_tcpFactory);
    QCOMPARE(range.m_available, false);
    QCOMPARE(range.m_urValue, 0.0);
}

void test_cro_range::setupServers()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

void test_cro_range::setupClient()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(netInfo, m_tcpFactory);
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

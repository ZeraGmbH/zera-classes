#include "test_cro_channelcurrentrange.h"
#include "cro_channelcurrentrange.h"
#include <testfactoryi2cctrl.h>
#include <networkconnectioninfo.h>
#include <timemachineobject.h>
#include <clampfactorytest.h>
#include <mocktcpnetworkfactory.h>
#include <mocki2ceepromiofactory.h>
#include <clamp.h>
#include <proxy.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_cro_channelcurrentrange)

static const NetworkConnectionInfo netInfo("127.0.0.1", 6307);

using namespace ChannelRangeObserver;

void test_cro_channelcurrentrange::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_cro_channelcurrentrange::init()
{
    setupServers();
    setupClient();
}

void test_cro_channelcurrentrange::cleanup()
{
    m_pcbInterface = nullptr;
    m_pcbClient = nullptr;
    TimeMachineObject::feedEventLoop();
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_cro_channelcurrentrange::noRangeOnStartup()
{
    ChannelCurrentRange ccr("m0", netInfo, m_tcpFactory);
    QVERIFY(ccr.getCurrentRange().isEmpty());
}

void test_cro_channelcurrentrange::signalsForValidFetch()
{
    ChannelCurrentRange ccr("m0", netInfo, m_tcpFactory);
    QSignalSpy spy(&ccr, &ChannelCurrentRange::sigFetchComplete);

    ccr.startObserve();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], "250V");
    QCOMPARE(spy[0][2], true);
}

void test_cro_channelcurrentrange::signalsForInvalidPort()
{
    const NetworkConnectionInfo netInfoInvalid("127.0.0.1", 42);
    ChannelCurrentRange ccr("m0", netInfoInvalid, m_tcpFactory);
    QSignalSpy spy(&ccr, &ChannelCurrentRange::sigFetchComplete);

    ccr.startObserve();
    TimeMachineForTest::getInstance()->processTimers(CONNECTION_TIMEOUT);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], "");
    QCOMPARE(spy[0][2], false);
}

void test_cro_channelcurrentrange::signalsForInvalidChannel()
{
    ChannelCurrentRange ccr("foo", netInfo, m_tcpFactory);
    QSignalSpy spy(&ccr, &ChannelCurrentRange::sigFetchComplete);

    ccr.startObserve();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "foo");
    QCOMPARE(spy[0][1], "");
    QCOMPARE(spy[0][2], false);
}

void test_cro_channelcurrentrange::getForValidFetch()
{
    ChannelCurrentRange ccr("m0", netInfo, m_tcpFactory);

    ccr.startObserve();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(ccr.getCurrentRange(), "250V");
}

void test_cro_channelcurrentrange::handleRangeChange()
{
    ChannelCurrentRange ccr("m0", netInfo, m_tcpFactory);
    ccr.startObserve();
    TimeMachineObject::feedEventLoop();

    QSignalSpy spy(&ccr, &ChannelCurrentRange::sigFetchComplete);
    m_pcbInterface->setRange("m0", "8V");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], "8V");
    QCOMPARE(spy[0][2], true);
    QCOMPARE(ccr.getCurrentRange(), "8V");
}

void test_cro_channelcurrentrange::afterRangeChange()
{
    ChannelCurrentRange ccr("m0", netInfo, m_tcpFactory);

    ccr.startObserve();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(ccr.getCurrentRange(), "250V");
}


void test_cro_channelcurrentrange::setupServers()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

void test_cro_channelcurrentrange::setupClient()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(netInfo, m_tcpFactory);
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

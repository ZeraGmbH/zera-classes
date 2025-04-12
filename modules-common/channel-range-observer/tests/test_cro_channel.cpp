#include "test_cro_channel.h"
#include "cro_channel.h"
#include "cro_channelfetchtask.h"
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

QTEST_MAIN(test_cro_channel)

static const NetworkConnectionInfo netInfo("127.0.0.1", 6307);

using namespace ChannelRangeObserver;

void test_cro_channel::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_cro_channel::init()
{
    setupServers();
    setupClient();
}

void test_cro_channel::cleanup()
{
    m_pcbInterface = nullptr;
    m_pcbClient = nullptr;
    TimeMachineObject::feedEventLoop();
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_cro_channel::emptyOnStartup()
{
    Channel channel("m0", netInfo, m_tcpFactory);
    QVERIFY(channel.getAllRangeNames().isEmpty());
}

void test_cro_channel::fetchDirect()
{
    Channel channel("m0", netInfo, m_tcpFactory);
    QSignalSpy spy(&channel, &Channel::sigFetchDoneChannel);

    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], true);
}

void test_cro_channel::fetchDirectTwice()
{
    Channel channel("m0", netInfo, m_tcpFactory);
    QSignalSpy spy(&channel, &Channel::sigFetchDoneChannel);

    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
}

void test_cro_channel::fetchInvalidChannel()
{
    Channel channel("foo", netInfo, m_tcpFactory);
    QSignalSpy spy(&channel, &Channel::sigFetchDoneChannel);

    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "foo");
    QCOMPARE(spy[0][1], false);
    QCOMPARE(channel.getAvailRangeNames(), QStringList());
}

void test_cro_channel::fetchByTaskValid()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory);
    ChannelFetchTaskPtr task = ChannelFetchTask::create(channel);

    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], true);
}

void test_cro_channel::fetchByTaskInvalid()
{
    ChannelPtr channel = std::make_shared<Channel>("foo", netInfo, m_tcpFactory);
    ChannelFetchTaskPtr task = ChannelFetchTask::create(channel);

    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], false);
}

void test_cro_channel::fetchCheckChannelDataM0()
{
    Channel channel("m0", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(channel.getAlias(), "UL1");
    QCOMPARE(channel.getUnit(), "V");
    QCOMPARE(channel.getDspChannel(), 0);
}

void test_cro_channel::fetchCheckChannelDataM3()
{
    Channel channel("m3", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(channel.getAlias(), "IL1");
    QCOMPARE(channel.getUnit(), "A");
    QCOMPARE(channel.getDspChannel(), 1);
}

void test_cro_channel::refetchAlthoughNotSuggestedWorks()
{
    Channel channel("m0", netInfo, m_tcpFactory);
    QSignalSpy spy(&channel, &Channel::sigFetchDoneChannel);

    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);

    spy.clear();
    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], true);

    QStringList rangeNamesReceived = channel.getAvailRangeNames();
    QStringList rangeNamesExpected = QStringList()
                                     << "250V" << "8V" << "100mV";
    QCOMPARE(rangeNamesReceived, rangeNamesExpected);
}

void test_cro_channel::getRangesCheckBasicData()
{
    Channel channel("m0", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QVERIFY(channel.getRange("250V") != nullptr);
    QCOMPARE(channel.getRange("foo"), nullptr);
    QStringList rangeName = channel.getAllRangeNames();
    QCOMPARE(rangeName.count(), 3);
    QVERIFY(rangeName.contains("8V"));
    QVERIFY(rangeName.contains("100mV"));
}

void test_cro_channel::checkUrValue()
{
    Channel channel("m0", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(*channel.getRange("250V")->m_urValue, 250.0);
    QCOMPARE(*channel.getRange("8V")->m_urValue, 8.0);
    QCOMPARE(*channel.getRange("100mV")->m_urValue, 0.1);
}

void test_cro_channel::checkOrderingVoltageRanges()
{
    Channel channel("m0", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeNamesReceived = channel.getAllRangeNames();
    QStringList rangeNamesExpected = QStringList() << "250V" << "8V" << "100mV";
    QCOMPARE(rangeNamesReceived, rangeNamesExpected);
}

void test_cro_channel::checkOrderingAllCurrentRanges()
{
    Channel channel("m3", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeNamesReceived = channel.getAllRangeNames();
    QStringList rangeNamesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA"
                                     << "8V" << "5V" << "2V" << "1V"
                                     << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";
    QCOMPARE(rangeNamesReceived, rangeNamesExpected);
}

void test_cro_channel::checkRangeAvailable()
{
    Channel channel("m3", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(channel.getRange("10A")->m_available, true);
    QCOMPARE(channel.getRange("500mA")->m_available, true);
    QCOMPARE(channel.getRange("8V")->m_available, false);
    QCOMPARE(channel.getRange("500mV")->m_available, false);
}

void test_cro_channel::checkAvailableRangesMtDefaultAc()
{
    Channel channel("m3", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeNamesReceived = channel.getAvailRangeNames();
    QStringList rangeNamesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeNamesReceived, rangeNamesExpected);
}

void test_cro_channel::checkAvailableRangesMtAdj()
{
    Channel channel("m3", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QSignalSpy spy(&channel, &Channel::sigFetchDoneChannel);
    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);

    QStringList rangeNamesReceivedAdj = channel.getAvailRangeNames();
    QStringList rangeNamesExpectedAdj = QStringList()
                                        << "10A" << "5A" << "2.5A" << "1.0A"
                                        << "500mA" << "250mA" << "100mA" << "50mA" << "25mA"
                                        << "8V" << "5V" << "2V" << "1V"
                                        << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";
    QCOMPARE(rangeNamesReceivedAdj, rangeNamesExpectedAdj);
}

void test_cro_channel::checkAvailableRangesMtSequence()
{
    Channel channel("m3", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeNamesReceived = channel.getAvailRangeNames();
    QStringList rangeNamesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeNamesReceived, rangeNamesExpected);

    QSignalSpy spy(&channel, &Channel::sigFetchDoneChannel);
    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);

    QStringList rangeNamesReceivedAdj = channel.getAvailRangeNames();
    QStringList rangeNamesExpectedAdj = QStringList()
                                        << "10A" << "5A" << "2.5A" << "1.0A"
                                        << "500mA" << "250mA" << "100mA" << "50mA" << "25mA"
                                        << "8V" << "5V" << "2V" << "1V"
                                        << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";
    QCOMPARE(rangeNamesReceivedAdj, rangeNamesExpectedAdj);

    m_pcbInterface->setMMode("AC");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 2);

    QStringList rangeNamesReceivedAc = channel.getAvailRangeNames();
    QStringList rangeNamesExpectedAc = QStringList()
                                        << "10A" << "5A" << "2.5A" << "1.0A"
                                        << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeNamesReceivedAc, rangeNamesExpectedAc);
}

void test_cro_channel::checkScanTwiceAvailableRangesMtAdj()
{
    Channel channel("m3", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QSignalSpy spy(&channel, &Channel::sigFetchDoneChannel);
    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
}

void test_cro_channel::notifyRangeChangeByClampComeAndGo()
{
    Channel channel("m3", netInfo, m_tcpFactory);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    QSignalSpy spy(&channel, &Channel::sigRangeChangeReported);

    spy.clear();
    m_testServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy[0][0], "m3");
    QCOMPARE(spy[0][1], 1);

    spy.clear();
    m_testServer->removeAllClamps();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy[0][0], "m3");
    QCOMPARE(spy[0][1], 2);

    spy.clear();
    m_testServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy[0][0], "m3");
    QCOMPARE(spy[0][1], 3);
}

void test_cro_channel::setupServers()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

void test_cro_channel::setupClient()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(netInfo, m_tcpFactory);
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

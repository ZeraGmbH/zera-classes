#include "test_cro_channel.h"
#include "cro_channel.h"
#include "cro_channelfetchtask.h"
#include <testfactoryi2cctrl.h>
#include <networkconnectioninfo.h>
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
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
    TimeMachineObject::feedEventLoop();
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_cro_channel::emptyOnStartup()
{
    Channel channel("m0", netInfo, m_tcpFactory, false);
    QVERIFY(channel.getAllRangeAliases().isEmpty());
}

void test_cro_channel::fetchDirect()
{
    Channel channel("m0", netInfo, m_tcpFactory, false);
    QSignalSpy spy(&channel, &Channel::sigNewRanges);

    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], true);
}

void test_cro_channel::fetchDirectTwice()
{
    Channel channel("m0", netInfo, m_tcpFactory, false);
    QSignalSpy spy(&channel, &Channel::sigNewRanges);

    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
}

void test_cro_channel::fetchInvalidChannel()
{
    Channel channel("foo", netInfo, m_tcpFactory, false);
    QSignalSpy spy(&channel, &Channel::sigNewRanges);

    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "foo");
    QCOMPARE(spy[0][1], false);
    QCOMPARE(channel.getAvailRangeAliases(), QStringList());
}

void test_cro_channel::fetchByTaskValid()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, false);
    ChannelFetchTaskPtr task = ChannelFetchTask::create(channel);

    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], true);
}

void test_cro_channel::fetchByTaskInvalid()
{
    ChannelPtr channel = std::make_shared<Channel>("foo", netInfo, m_tcpFactory, false);
    ChannelFetchTaskPtr task = ChannelFetchTask::create(channel);

    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], false);
}

void test_cro_channel::fetchCheckChannelDataM0()
{
    Channel channel("m0", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(channel.getAlias(), "UL1");
    QCOMPARE(channel.getUnit(), "V");
    QCOMPARE(channel.getDspChannel(), 0);
}

void test_cro_channel::fetchCheckChannelDataM3()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(channel.getAlias(), "IL1");
    QCOMPARE(channel.getUnit(), "A");
    QCOMPARE(channel.getDspChannel(), 1);
}

void test_cro_channel::refetchAlthoughNotSuggestedWorks()
{
    Channel channel("m0", netInfo, m_tcpFactory, false);
    QSignalSpy spy(&channel, &Channel::sigNewRanges);

    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);

    spy.clear();
    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], true);

    QStringList rangeAliasesReceived = channel.getAvailRangeAliases();
    QStringList rangeAliasesExpected = QStringList()
                                     << "250V" << "8V" << "100mV";
    QCOMPARE(rangeAliasesReceived, rangeAliasesExpected);
}

void test_cro_channel::getRangesCheckBasicData()
{
    Channel channel("m0", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QVERIFY(channel.getRange("250V") != nullptr);
    QVERIFY(channel.getRange("foo") != nullptr); // try sane fallback
    QStringList rangeName = channel.getAllRangeAliases();
    QCOMPARE(rangeName.count(), 3);
    QVERIFY(rangeName.contains("8V"));
    QVERIFY(rangeName.contains("100mV"));
}

void test_cro_channel::checkUrValue()
{
    Channel channel("m0", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(channel.getRange("250V")->getUrValue(), 250.0);
    QCOMPARE(channel.getRange("8V")->getUrValue(), 8.0);
    QCOMPARE(channel.getRange("100mV")->getUrValue(), 0.1);
}

void test_cro_channel::checkOrderingVoltageRanges()
{
    Channel channel("m0", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeAliasesReceived = channel.getAllRangeAliases();
    QStringList rangeAliasesExpected = QStringList() << "250V" << "8V" << "100mV";
    QCOMPARE(rangeAliasesReceived, rangeAliasesExpected);
}

void test_cro_channel::checkOrderingCurrentRanges()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeAliasesReceived = channel.getAvailRangeAliases();
    QStringList rangeAliasesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeAliasesReceived, rangeAliasesExpected);
}

void test_cro_channel::checkOrderingAllCurrentRanges()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeAliasesReceived = channel.getAllRangeAliases();
    QStringList rangeAliasesExpected = QStringList()
                                       << "8V" << "5V" << "2V" << "1V"
                                       << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV"
                                       << "10A" << "5A" << "2.5A" << "1.0A"
                                       << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeAliasesReceived, rangeAliasesExpected);
}

void test_cro_channel::checkRangeAvailable()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(channel.getRange("10A")->getAvailable(), true);
    QCOMPARE(channel.getRange("500mA")->getAvailable(), true);
    QCOMPARE(channel.getRange("8V")->getAvailable(), false);
    QCOMPARE(channel.getRange("500mV")->getAvailable(), false);
}

void test_cro_channel::checkAvailableRangesMtDefaultAc()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeAliasesReceived = channel.getAvailRangeAliases();
    QStringList rangeAliasesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeAliasesReceived, rangeAliasesExpected);
}

void test_cro_channel::checkAvailableRangesMtAdj()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QSignalSpy spy(&channel, &Channel::sigNewRanges);
    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);

    QStringList rangeAliasesReceivedAdj = channel.getAvailRangeAliases();
    QStringList rangeAliasesExpectedAdj = QStringList()
                                          << "8V" << "5V" << "2V" << "1V"
                                          << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV"
                                          << "10A" << "5A" << "2.5A" << "1.0A"
                                          << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeAliasesReceivedAdj, rangeAliasesExpectedAdj);
}

void test_cro_channel::checkAvailableRangesMtSequence()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeAliasesReceived = channel.getAvailRangeAliases();
    QStringList rangeAliasesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeAliasesReceived, rangeAliasesExpected);

    QSignalSpy spy(&channel, &Channel::sigNewRanges);
    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);

    QStringList rangeAliasesReceivedAdj = channel.getAvailRangeAliases();
    QStringList rangeAliasesExpectedAdj = QStringList()
                                          << "8V" << "5V" << "2V" << "1V"
                                          << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV"
                                          << "10A" << "5A" << "2.5A" << "1.0A"
                                          << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeAliasesReceivedAdj, rangeAliasesExpectedAdj);

    m_pcbInterface->setMMode("AC");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 2);

    QStringList rangeAliasesReceivedAc = channel.getAvailRangeAliases();
    QStringList rangeAliasesExpectedAc = QStringList()
                                        << "10A" << "5A" << "2.5A" << "1.0A"
                                        << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeAliasesReceivedAc, rangeAliasesExpectedAc);
}

void test_cro_channel::checkScanTwiceAvailableRangesMtAdj()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QSignalSpy spy(&channel, &Channel::sigNewRanges);
    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
}

void test_cro_channel::notifyFetchDoneByClampComeAndGo()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();
    QSignalSpy spy(&channel, &Channel::sigNewRanges);

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
    QCOMPARE(spy[0][1], true);

    spy.clear();
    m_testServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy[0][0], "m3");
    QCOMPARE(spy[0][1], true);
}

void test_cro_channel::checkAvailableRangesMtClampComeAndGo()
{
    Channel channel("m3", netInfo, m_tcpFactory, false);
    channel.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeAliasesReceived = channel.getAvailRangeAliases();
    QStringList rangeAliasesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeAliasesReceived, rangeAliasesExpected);

    m_testServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();

    QStringList rangeAliasesReceivedClamp = channel.getAvailRangeAliases();
    QStringList rangeAliasesExpectedClamp = QStringList()
                                          << "10A" << "5A" << "2.5A" << "1.0A"
                                          << "500mA" << "250mA" << "100mA" << "50mA" << "25mA"
                                          << "C100A" << "C50A" << "C10A" << "C5A" << "C1A"
                                          << "C500mA" << "C100mA" << "C50mA" << "C10mA";
    QCOMPARE(rangeAliasesReceivedClamp, rangeAliasesExpectedClamp);

    m_testServer->removeAllClamps();
    TimeMachineObject::feedEventLoop();

    rangeAliasesReceived = channel.getAvailRangeAliases();
    QCOMPARE(rangeAliasesReceived, rangeAliasesExpected);
}

void test_cro_channel::setupServers()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMtxxxs2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

void test_cro_channel::setupClient()
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSuperSmart(netInfo, m_tcpFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbInterface->getClientSmart());
    TimeMachineObject::feedEventLoop();
}

#include "test_cro_channel.h"
#include "channel.h"
#include "channelfetchtask.h"
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
    Channel observer("m0", netInfo, m_tcpFactory);
    QVERIFY(observer.getAllRangeNames().isEmpty());
}

void test_cro_channel::fetchDirect()
{
    Channel observer("m0", netInfo, m_tcpFactory);
    QSignalSpy spy(&observer, &Channel::sigFetchComplete);

    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_cro_channel::fetchDirectTwice()
{
    Channel observer("m0", netInfo, m_tcpFactory);
    QSignalSpy spy(&observer, &Channel::sigFetchComplete);

    observer.startFetch();
    TimeMachineObject::feedEventLoop();
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
}

void test_cro_channel::fetchByTask()
{
    ChannelPtr observer = std::make_shared<Channel>("m0", netInfo, m_tcpFactory);
    ChannelFetchTaskPtr task = ChannelFetchTask::create(observer);

    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_cro_channel::fetchCheckChannelDataM0()
{
    Channel observer("m0", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.m_alias, "UL1");
    QCOMPARE(observer.m_unit, "V");
    QCOMPARE(observer.m_dspChannel, 0);
}

void test_cro_channel::fetchCheckChannelDataM3()
{
    Channel observer("m3", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.m_alias, "IL1");
    QCOMPARE(observer.m_unit, "A");
    QCOMPARE(observer.m_dspChannel, 1);
}

void test_cro_channel::getRangesCheckBasicData()
{
    Channel observer("m0", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QVERIFY(observer.getRange("250V") != nullptr);
    QCOMPARE(observer.getRange("foo"), nullptr);
    QStringList rangeName = observer.getAllRangeNames();
    QCOMPARE(rangeName.count(), 3);
    QVERIFY(rangeName.contains("8V"));
    QVERIFY(rangeName.contains("100mV"));
}

void test_cro_channel::checkUrValue()
{
    Channel observer("m0", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getRange("250V")->m_urValue, 250.0);
    QCOMPARE(observer.getRange("8V")->m_urValue, 8.0);
    QCOMPARE(observer.getRange("100mV")->m_urValue, 0.1);
}

void test_cro_channel::checkOrderingVoltageRanges()
{
    Channel observer("m0", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeNamesReceived = observer.getAllRangeNames();
    QStringList rangeNamesExpected = QStringList() << "250V" << "8V" << "100mV";
    QCOMPARE(rangeNamesReceived, rangeNamesExpected);
}

void test_cro_channel::checkOrderingAllCurrentRanges()
{
    Channel observer("m3", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeNamesReceived = observer.getAllRangeNames();
    QStringList rangeNamesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA"
                                     << "8V" << "5V" << "2V" << "1V"
                                     << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";
    QCOMPARE(rangeNamesReceived, rangeNamesExpected);
}

void test_cro_channel::checkRangeAvailable()
{
    Channel observer("m3", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getRange("10A")->m_available, true);
    QCOMPARE(observer.getRange("500mA")->m_available, true);
    QCOMPARE(observer.getRange("8V")->m_available, false);
    QCOMPARE(observer.getRange("500mV")->m_available, false);
}

void test_cro_channel::checkAvailableRangesMtDefaultAc()
{
    Channel observer("m3", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeNamesReceived = observer.getAvailRangeNames();
    QStringList rangeNamesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeNamesReceived, rangeNamesExpected);
}

void test_cro_channel::checkAvailableRangesMtAdj()
{
    Channel observer("m3", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QSignalSpy spy(&observer, &Channel::sigFetchComplete);
    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);

    QStringList rangeNamesReceivedAdj = observer.getAvailRangeNames();
    QStringList rangeNamesExpectedAdj = QStringList()
                                        << "10A" << "5A" << "2.5A" << "1.0A"
                                        << "500mA" << "250mA" << "100mA" << "50mA" << "25mA"
                                        << "8V" << "5V" << "2V" << "1V"
                                        << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";
    QCOMPARE(rangeNamesReceivedAdj, rangeNamesExpectedAdj);
}

void test_cro_channel::checkAvailableRangesMtSequence()
{
    Channel observer("m3", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QStringList rangeNamesReceived = observer.getAvailRangeNames();
    QStringList rangeNamesExpected = QStringList()
                                     << "10A" << "5A" << "2.5A" << "1.0A"
                                     << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeNamesReceived, rangeNamesExpected);

    QSignalSpy spy(&observer, &Channel::sigFetchComplete);
    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);

    QStringList rangeNamesReceivedAdj = observer.getAvailRangeNames();
    QStringList rangeNamesExpectedAdj = QStringList()
                                        << "10A" << "5A" << "2.5A" << "1.0A"
                                        << "500mA" << "250mA" << "100mA" << "50mA" << "25mA"
                                        << "8V" << "5V" << "2V" << "1V"
                                        << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";
    QCOMPARE(rangeNamesReceivedAdj, rangeNamesExpectedAdj);

    m_pcbInterface->setMMode("AC");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 2);

    QStringList rangeNamesReceivedAc = observer.getAvailRangeNames();
    QStringList rangeNamesExpectedAc = QStringList()
                                        << "10A" << "5A" << "2.5A" << "1.0A"
                                        << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QCOMPARE(rangeNamesReceivedAc, rangeNamesExpectedAc);
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

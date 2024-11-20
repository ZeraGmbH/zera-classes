#include "test_channel_range_observer.h"
#include "allchannelobserver.h"
#include "allchannelobserverresetter.h"
#include "networkconnectioninfo.h"
#include <timemachineobject.h>
#include <testallservicesmt310s2.h>
#include <clampfactorytest.h>
#include <mocktcpnetworkfactory.h>
#include <mocki2ceepromiofactory.h>
#include <clamp.h>
#include <proxy.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QTest>
#include <QSignalSpy>

QTEST_MAIN(test_channel_range_observer)

static const NetworkConnectionInfo netInfo("127.0.0.1", 6307);

void test_channel_range_observer::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_channel_range_observer::init()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
    TimeMachineObject::feedEventLoop();
}

void test_channel_range_observer::cleanup()
{
    TimeMachineObject::feedEventLoop();
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_channel_range_observer::emptyChannelListOnStartup()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    QVERIFY(observer.getChannelMNames().isEmpty());
}

void test_channel_range_observer::scanChannelListSignalReturned()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    QSignalSpy spy(&observer, &AllChannelObserver::sigFullScanFinished);

    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_channel_range_observer::scanChannelListChannelsReturned()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QStringList channels = observer.getChannelMNames();
    QCOMPARE(channels.count(), 8);
    QVERIFY(channels.contains("m0"));
    QVERIFY(channels.contains("m1"));
    QVERIFY(channels.contains("m2"));
    QVERIFY(channels.contains("m3"));
    QVERIFY(channels.contains("m4"));
    QVERIFY(channels.contains("m5"));
    QVERIFY(channels.contains("m6"));
    QVERIFY(channels.contains("m7"));
}

void test_channel_range_observer::rescanWithoutClear()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    QSignalSpy spyRangesChanged(&observer, &AllChannelObserver::sigRangeListChanged);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyRangesChanged.count(), 8);
    QStringList channels = observer.getChannelMNames();
    QCOMPARE(channels.count(), 8);

    spyRangesChanged.clear();
    cleanup(); // make sure no I/O is performed by killing servers

    QSignalSpy spyFinished(&observer, &AllChannelObserver::sigFullScanFinished);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannelMNames().count(), 8);
    QCOMPARE(spyRangesChanged.count(), 0);
    QCOMPARE(spyFinished.count(), 1);
}

void test_channel_range_observer::rescanWithClear()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    QSignalSpy spyRangesChanged(&observer, &AllChannelObserver::sigRangeListChanged);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyRangesChanged.count(), 8);
    QStringList channels = observer.getChannelMNames();
    QCOMPARE(channels.count(), 8);

    spyRangesChanged.clear();
    AllChannelObserverResetter::clear(&observer);
    QCOMPARE(observer.getChannelMNames().count(), 0);

    QSignalSpy spyFinished(&observer, &AllChannelObserver::sigFullScanFinished);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannelMNames().count(), 8);
    QCOMPARE(spyRangesChanged.count(), 8);
    QCOMPARE(spyFinished.count(), 1);
}

void test_channel_range_observer::checkAlias()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannelData("m0")->m_alias, "UL1");
    QCOMPARE(observer.getChannelData("m1")->m_alias, "UL2");
    QCOMPARE(observer.getChannelData("m2")->m_alias, "UL3");
    QCOMPARE(observer.getChannelData("m3")->m_alias, "IL1");
    QCOMPARE(observer.getChannelData("m4")->m_alias, "IL2");
    QCOMPARE(observer.getChannelData("m5")->m_alias, "IL3");
    QCOMPARE(observer.getChannelData("m6")->m_alias, "UAUX");
    QCOMPARE(observer.getChannelData("m7")->m_alias, "IAUX");
}

void test_channel_range_observer::checkDspChannel()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannelData("m0")->m_dspChannel, 0);
    QCOMPARE(observer.getChannelData("m1")->m_dspChannel, 2);
    QCOMPARE(observer.getChannelData("m2")->m_dspChannel, 4);
    QCOMPARE(observer.getChannelData("m3")->m_dspChannel, 1);
    QCOMPARE(observer.getChannelData("m4")->m_dspChannel, 3);
    QCOMPARE(observer.getChannelData("m5")->m_dspChannel, 5);
    QCOMPARE(observer.getChannelData("m6")->m_dspChannel, 6);
    QCOMPARE(observer.getChannelData("m7")->m_dspChannel, 7);
}

void test_channel_range_observer::checkUnit()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannelData("m0")->m_unit, "V");
    QCOMPARE(observer.getChannelData("m3")->m_unit, "A");
}

void test_channel_range_observer::checkRanges()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannelRanges("m0").size(), 3);
    QCOMPARE(observer.getChannelRanges("m0")[0], "250V");
}

void test_channel_range_observer::changeRangesByClamp()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();
    QStringList ranges = observer.getChannelRanges("m3");
    QCOMPARE(ranges.size(), 21);

    m_testServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();

    ranges = observer.getChannelRanges("m3");
    QCOMPARE(ranges.size(), 30);

    TimeMachineObject::feedEventLoop();
}

void test_channel_range_observer::notifyRangeChangeByClamp()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    QSignalSpy spy(&observer, &AllChannelObserver::sigRangeListChanged);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 8);

    spy.clear();
    m_testServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 1);

    spy.clear();
    m_testServer->addClamp(cClamp::CL120A, "IL2");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 1);

    spy.clear();
    m_testServer->removeAllClamps();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 2);
}

void test_channel_range_observer::getDataForInvalidChannel()
{
    AllChannelObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannelData("foo"), nullptr);
}

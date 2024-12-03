#include "test_cro_system_observer.h"
#include "cro_systemobserver.h"
#include "cro_systemobserverfetchtask.h"
#include "cro_systemobserverresetter.h"
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
#include <QTest>
#include <QSignalSpy>

QTEST_MAIN(test_cro_system_observer)

static const NetworkConnectionInfo netInfo("127.0.0.1", 6307);

using namespace ChannelRangeObserver;

void test_cro_system_observer::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_cro_system_observer::init()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
    TimeMachineObject::feedEventLoop();
}

void test_cro_system_observer::cleanup()
{
    TimeMachineObject::feedEventLoop();
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_cro_system_observer::emptyChannelListOnStartup()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    QVERIFY(observer.getChannelMNames().isEmpty());
}

void test_cro_system_observer::scanChannelListSignalReturned()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    QSignalSpy spy(&observer, &SystemObserver::sigFullScanFinished);

    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], true);
}

void test_cro_system_observer::scanChannelListChannelsReturned()
{
    SystemObserver observer(netInfo, m_tcpFactory);
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

void test_cro_system_observer::scanByTaskValid()
{
    SystemObserverPtr observer = std::make_shared<SystemObserver>(netInfo, m_tcpFactory);
    SystemObserverFetchTaskPtr task = SystemObserverFetchTask::create(observer);
    QSignalSpy spy(task.get(), &SystemObserverFetchTask::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], true);
}

void test_cro_system_observer::scanByTaskInvalid()
{
    NetworkConnectionInfo netInfoInvalid("127.0.0.1", 42);
    SystemObserverPtr observer = std::make_shared<SystemObserver>(netInfoInvalid, m_tcpFactory);
    SystemObserverFetchTaskPtr task = SystemObserverFetchTask::create(observer);
    QSignalSpy spy(task.get(), &SystemObserverFetchTask::sigFinish);
    task->start();
    TimeMachineForTest::getInstance()->processTimers(CONNECTION_TIMEOUT);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], false);
}

void test_cro_system_observer::rescanWithoutClear()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    QSignalSpy spyRangesChanged(&observer, &SystemObserver::sigFetchComplete);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyRangesChanged.count(), 8);
    QStringList channels = observer.getChannelMNames();
    QCOMPARE(channels.count(), 8);

    spyRangesChanged.clear();
    cleanup(); // make sure no I/O is performed by killing servers

    QSignalSpy spyFinished(&observer, &SystemObserver::sigFullScanFinished);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannelMNames().count(), 8);
    QCOMPARE(spyRangesChanged.count(), 0);
    QCOMPARE(spyFinished.count(), 1);
    QCOMPARE(spyFinished[0][0], true);
}

void test_cro_system_observer::rescanWithClear()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    QSignalSpy spyRangesChanged(&observer, &SystemObserver::sigFetchComplete);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyRangesChanged.count(), 8);
    QCOMPARE(spyRangesChanged[0][1], true);
    QCOMPARE(spyRangesChanged[1][1], true);
    QCOMPARE(spyRangesChanged[2][1], true);
    QCOMPARE(spyRangesChanged[3][1], true);
    QCOMPARE(spyRangesChanged[4][1], true);
    QCOMPARE(spyRangesChanged[5][1], true);
    QCOMPARE(spyRangesChanged[6][1], true);
    QCOMPARE(spyRangesChanged[7][1], true);
    QStringList channels = observer.getChannelMNames();
    QCOMPARE(channels.count(), 8);

    spyRangesChanged.clear();
    SystemObserverResetter::clear(&observer);
    QCOMPARE(observer.getChannelMNames().count(), 0);

    QSignalSpy spyFinished(&observer, &SystemObserver::sigFullScanFinished);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannelMNames().count(), 8);
    QCOMPARE(spyRangesChanged.count(), 8);
    QCOMPARE(spyRangesChanged.count(), 8);
    QCOMPARE(spyRangesChanged[0][1], true);
    QCOMPARE(spyRangesChanged[1][1], true);
    QCOMPARE(spyRangesChanged[2][1], true);
    QCOMPARE(spyRangesChanged[3][1], true);
    QCOMPARE(spyRangesChanged[4][1], true);
    QCOMPARE(spyRangesChanged[5][1], true);
    QCOMPARE(spyRangesChanged[6][1], true);
    QCOMPARE(spyRangesChanged[7][1], true);
    QCOMPARE(spyFinished.count(), 1);
    QCOMPARE(spyFinished[0][0], true);
}

void test_cro_system_observer::checkSampleRate()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getSampleRate(), 504);
}

void test_cro_system_observer::checkAlias()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannel("m0")->m_alias, "UL1");
    QCOMPARE(observer.getChannel("m1")->m_alias, "UL2");
    QCOMPARE(observer.getChannel("m2")->m_alias, "UL3");
    QCOMPARE(observer.getChannel("m3")->m_alias, "IL1");
    QCOMPARE(observer.getChannel("m4")->m_alias, "IL2");
    QCOMPARE(observer.getChannel("m5")->m_alias, "IL3");
    QCOMPARE(observer.getChannel("m6")->m_alias, "UAUX");
    QCOMPARE(observer.getChannel("m7")->m_alias, "IAUX");
}

void test_cro_system_observer::checkDspChannel()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannel("m0")->m_dspChannel, 0);
    QCOMPARE(observer.getChannel("m1")->m_dspChannel, 2);
    QCOMPARE(observer.getChannel("m2")->m_dspChannel, 4);
    QCOMPARE(observer.getChannel("m3")->m_dspChannel, 1);
    QCOMPARE(observer.getChannel("m4")->m_dspChannel, 3);
    QCOMPARE(observer.getChannel("m5")->m_dspChannel, 5);
    QCOMPARE(observer.getChannel("m6")->m_dspChannel, 6);
    QCOMPARE(observer.getChannel("m7")->m_dspChannel, 7);
}

void test_cro_system_observer::checkUnit()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannel("m0")->m_unit, "V");
    QCOMPARE(observer.getChannel("m3")->m_unit, "A");
}

void test_cro_system_observer::checkRanges()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannel("m0")->getAllRangeNames().size(), 3);
    QCOMPARE(observer.getChannel("m0")->getAllRangeNames()[0], "250V");
}

void test_cro_system_observer::changeRangesByClamp()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();
    QStringList ranges = observer.getChannel("m3")->getAllRangeNames();
    QCOMPARE(ranges.size(), 21);

    m_testServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();

    ranges = observer.getChannel("m3")->getAllRangeNames();
    QCOMPARE(ranges.size(), 30);

    TimeMachineObject::feedEventLoop();
}

void test_cro_system_observer::notifyRangeChangeByClamp()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    QSignalSpy spy(&observer, &SystemObserver::sigFetchComplete);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 8);

    spy.clear();
    m_testServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy[0][0], "m3");
    QCOMPARE(spy[0][1], true);

    spy.clear();
    m_testServer->addClamp(cClamp::CL120A, "IL2");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy[0][0], "m4");
    QCOMPARE(spy[0][1], true);

    spy.clear();
    m_testServer->removeAllClamps();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.size(), 2);
    QCOMPARE(spy[0][1], true);
    QCOMPARE(spy[1][1], true);
}

void test_cro_system_observer::getDataForInvalidChannel()
{
    SystemObserver observer(netInfo, m_tcpFactory);
    observer.startFullScan();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.getChannel("foo"), nullptr);
}

void test_cro_system_observer::invalidScan()
{
    NetworkConnectionInfo netInfoInvalid("127.0.0.1", 42);
    SystemObserver observer(netInfoInvalid, m_tcpFactory);
    QSignalSpy spyFullScanFinished(&observer, &SystemObserver::sigFullScanFinished);
    QSignalSpy spyChannel(&observer, &SystemObserver::sigFetchComplete);
    observer.startFullScan();
    TimeMachineForTest::getInstance()->processTimers(CONNECTION_TIMEOUT);

    QCOMPARE(spyFullScanFinished.size(), 1);
    QCOMPARE(spyFullScanFinished[0][0], false);
    QCOMPARE(spyChannel.count(), 0);
}

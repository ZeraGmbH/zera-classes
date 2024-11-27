#include "test_cro_range.h"
#include "cro_range.h"
#include "cro_rangefetchtask.h"
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

void test_cro_range::fetchAvailable()
{
    Range range("m0", "250V", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], "250V");
    QCOMPARE(spy[0][2], true);
    QCOMPARE(range.m_available, true);
}

void test_cro_range::fetchNotAvailable()
{
    Range range("m3", "2mV", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m3");
    QCOMPARE(spy[0][1], "2mV");
    QCOMPARE(spy[0][2], true);
    QCOMPARE(range.m_available, false);
}

void test_cro_range::fetchInvalidChannel()
{
    Range range("foo", "5000V", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "foo");
    QCOMPARE(spy[0][1], "5000V");
    QCOMPARE(spy[0][2], false);
    QCOMPARE(range.m_available, false);
}

void test_cro_range::fetchInvalidRange()
{
    Range range("m0", "foo", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], "foo");
    QCOMPARE(spy[0][2], false);
    QCOMPARE(range.m_available, false);
}

void test_cro_range::fetchByTaskValid()
{
    RangePtr range = std::make_shared<Range>("m0", "250V", netInfo, m_tcpFactory);
    RangeFetchTaskPtr task = RangeFetchTask::create(range);

    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], true);
}

void test_cro_range::fetchByTaskInvalid()
{
    RangePtr range = std::make_shared<Range>("m0", "foo", netInfo, m_tcpFactory);
    RangeFetchTaskPtr task = RangeFetchTask::create(range);

    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], false);
}

void test_cro_range::refetchAlthoughNotSuggestedWorks()
{
    Range range("m0", "250V", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);

    range.startFetch();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    spy.clear();

    range.startFetch();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], "250V");
    QCOMPARE(spy[0][2], true);
    QCOMPARE(range.m_available, true);
}

void test_cro_range::fetchUrValueMatching()
{
    Range range("m0", "250V", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(range.m_urValue, 250);
}

void test_cro_range::setupServers()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

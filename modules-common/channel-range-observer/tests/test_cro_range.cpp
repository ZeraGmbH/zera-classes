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

void test_cro_range::fetchTwice()
{
    Range range("m0", "250V", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);

    range.startFetch();
    TimeMachineObject::feedEventLoop();
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy[0][0], "m0");
    QCOMPARE(spy[0][1], "250V");
    QCOMPARE(spy[0][2], true);
    QCOMPARE(spy[1][0], "m0");
    QCOMPARE(spy[1][1], "250V");
    QCOMPARE(spy[1][2], true);
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

void test_cro_range::checkUrValue()
{
    Range range("m0", "250V", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(range.m_urValue, 250);
}

void test_cro_range::checkType()
{
    // stolen from zenux-services/mt310s2senseinterface.cpp
    enum SensorType {
        Direct = 0x100,
        Clamp = 0x200
    };
    enum MMode
    {
        modeAC = 1,
        modeHF = 2,
        modeDC = 4,
        modeADJ = 8
    };

    // external
    Range range1("m0", "250V", netInfo, m_tcpFactory);
    range1.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(range1.m_type, Direct + modeAC + modeADJ);

    // internal clamp voltage range (don't show up on mode 'AC')
    Range range2("m3", "2mV", netInfo, m_tcpFactory);
    range2.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(range2.m_type, Direct + modeADJ);
}

void test_cro_range::checkRejection()
{
    Range range("m0", "250V", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    // Value taken from Mt310s2SenseInterface::setChannelAndRanges
    QCOMPARE(range.m_rejection, 4415057.0);
}

void test_cro_range::checkOvRejection()
{
    Range range("m0", "250V", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    // Value taken from Mt310s2SenseInterface::setChannelAndRanges
    QCOMPARE(range.m_ovrejection, 5518821.0);
}

void test_cro_range::checkAdcRejection()
{
    Range range("m0", "250V", netInfo, m_tcpFactory);
    QSignalSpy spy(&range, &Range::sigFetchComplete);
    range.startFetch();
    TimeMachineObject::feedEventLoop();

    // check mt310s2senserange.cpp / adcRejectionMt310s2
    QCOMPARE(range.m_adcrejection, 8388607.0);
}

void test_cro_range::setupServers()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

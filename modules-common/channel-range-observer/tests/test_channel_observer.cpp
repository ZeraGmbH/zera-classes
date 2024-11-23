#include "test_channel_observer.h"
#include "channelbserver.h"
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
#include <QTest>
#include <QSignalSpy>

QTEST_MAIN(test_channel_observer)

static const NetworkConnectionInfo netInfo("127.0.0.1", 6307);

void test_channel_observer::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_channel_observer::init()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
    TimeMachineObject::feedEventLoop();
}

void test_channel_observer::cleanup()
{
    TimeMachineObject::feedEventLoop();
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_channel_observer::emptyOnStartup()
{
    ChannelObserver observer("m0", netInfo, m_tcpFactory);
    QVERIFY(observer.getRangeNames().isEmpty());
}

void test_channel_observer::fetchDirect()
{
    ChannelObserver observer("m0", netInfo, m_tcpFactory);
    QSignalSpy spy(&observer, &ChannelObserver::sigFetchComplete);

    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_channel_observer::fetchDirectTwice()
{
    ChannelObserver observer("m0", netInfo, m_tcpFactory);
    QSignalSpy spy(&observer, &ChannelObserver::sigFetchComplete);

    observer.startFetch();
    TimeMachineObject::feedEventLoop();
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
}

void test_channel_observer::fetchByTask()
{
    ChannelObserverPtr observer = std::make_shared<ChannelObserver>("m0", netInfo, m_tcpFactory);
    ChannelFetchTaskPtr task = ChannelFetchTask::create(observer);

    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_channel_observer::fetchCheckChannelDataM0()
{
    ChannelObserver observer("m0", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.m_alias, "UL1");
    QCOMPARE(observer.m_unit, "V");
    QCOMPARE(observer.m_dspChannel, 0);
}

void test_channel_observer::fetchCheckChannelDataM3()
{
    ChannelObserver observer("m3", netInfo, m_tcpFactory);
    observer.startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.m_alias, "IL1");
    QCOMPARE(observer.m_unit, "A");
    QCOMPARE(observer.m_dspChannel, 1);
}


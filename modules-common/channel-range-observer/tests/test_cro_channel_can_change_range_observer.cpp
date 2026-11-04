#include "test_cro_channel_can_change_range_observer.h"
#include "cro_channelcanchangerangeobserver.h"
#include <networkconnectioninfo.h>
#include <mocktcpnetworkfactory.h>
#include <testfactoryi2cctrl.h>
#include <timerfactoryqtfortest.h>
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>
#include <vs_storagecomponent.h>

QTEST_MAIN(test_cro_channel_can_change_range_observer)

static const NetworkConnectionInfo netInfo("127.0.0.1", 6307);

using namespace ChannelRangeObserver;
using namespace VeinStorage;

void test_cro_channel_can_change_range_observer::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
    qputenv("QT_FATAL_CRITICALS", "1");
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
}

void test_cro_channel_can_change_range_observer::init()
{
    setupServers();
}

void test_cro_channel_can_change_range_observer::cleanup()
{
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_cro_channel_can_change_range_observer::cannotSetRangesAfterInit()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, false);
    ChannelCanChangeRangeObserver observer(channel, nullptr);
    QCOMPARE(observer.canChangeRange(), false);
}

void test_cro_channel_can_change_range_observer::canSetRangesChannelsFetched()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, false);
    channel->startFetch();
    TimeMachineObject::feedEventLoop();
    ChannelCanChangeRangeObserver observer(channel, nullptr);

    QCOMPARE(observer.canChangeRange(), true);
}

void test_cro_channel_can_change_range_observer::cannotSetRangesChannelsFetchedOneRange()
{
    ChannelPtr channel = std::make_shared<Channel>("m7", netInfo, m_tcpFactory, false); // IAUX / no clamps => only 0A
    channel->startFetch();
    TimeMachineObject::feedEventLoop();
    ChannelCanChangeRangeObserver observer(channel, nullptr);

    QCOMPARE(observer.canChangeRange(), false);
}

void test_cro_channel_can_change_range_observer::canSetRangesAfterChannelsFetched()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, false);
    ChannelCanChangeRangeObserver observer(channel, nullptr);
    QSignalSpy spy(&observer, &ChannelCanChangeRangeObserver::sigCanChangeRangeChanged);

    channel->startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.canChangeRange(), true);
    QCOMPARE(spy.count(), 1);
}

void test_cro_channel_can_change_range_observer::cannotSetRangesAfterChannelsFetched()
{
    ChannelPtr channel = std::make_shared<Channel>("m7", netInfo, m_tcpFactory, false); // IAUX / no clamps => only 0A
    ChannelCanChangeRangeObserver observer(channel, nullptr);
    QSignalSpy spy(&observer, &ChannelCanChangeRangeObserver::sigCanChangeRangeChanged);

    channel->startFetch();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.canChangeRange(), false);
    QCOMPARE(spy.count(), 0);
}

void test_cro_channel_can_change_range_observer::cannotChangeInitialRangeAutomaticIsOn()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, false);
    channel->startFetch();
    TimeMachineObject::feedEventLoop();
    AbstractComponentPtr rangeAutoComponent = std::make_shared<StorageComponent>(int(1));
    ChannelCanChangeRangeObserver observer(channel, rangeAutoComponent);

    QCOMPARE(observer.canChangeRange(), false);
}

void test_cro_channel_can_change_range_observer::cannotChangeAfterRangeAutomaticIsOn()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, false);
    channel->startFetch();
    TimeMachineObject::feedEventLoop();
    AbstractComponentPtr rangeAutoComponent = std::make_shared<StorageComponent>(int(0));
    ChannelCanChangeRangeObserver observer(channel, rangeAutoComponent);

    QCOMPARE(observer.canChangeRange(), true);

    rangeAutoComponent->setValue(1);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(observer.canChangeRange(), false);
}

void test_cro_channel_can_change_range_observer::canChangeAfterRangeAutomaticIsOnOff()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, false);
    channel->startFetch();
    TimeMachineObject::feedEventLoop();
    AbstractComponentPtr rangeAutoComponent = std::make_shared<StorageComponent>(int(0));
    ChannelCanChangeRangeObserver observer(channel, rangeAutoComponent);

    QCOMPARE(observer.canChangeRange(), true);

    rangeAutoComponent->setValue(1);
    rangeAutoComponent->setValue(0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(observer.canChangeRange(), true);
}

void test_cro_channel_can_change_range_observer::canChangeInitialSourceModeIsOff()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, true);
    channel->startFetch();
    TimeMachineObject::feedEventLoop();
    m_testServer->setSourceOnMode(QStringList());
    TimeMachineObject::feedEventLoop();
    ChannelCanChangeRangeObserver observer(channel, nullptr);

    QCOMPARE(observer.canChangeRange(), true);
}

void test_cro_channel_can_change_range_observer::cannotChangeInitialSourceModeIsOn()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, true);
    channel->startFetch();
    TimeMachineObject::feedEventLoop();
    m_testServer->setSourceOnMode(QStringList() << "m0");
    TimeMachineObject::feedEventLoop();
    ChannelCanChangeRangeObserver observer(channel, nullptr);

    QCOMPARE(observer.canChangeRange(), false);
}

void test_cro_channel_can_change_range_observer::cannotChangeAfterSourceModeIsOn()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, true);
    channel->startFetch();
    TimeMachineObject::feedEventLoop();
    ChannelCanChangeRangeObserver observer(channel, nullptr);

    QCOMPARE(observer.canChangeRange(), true);

    m_testServer->setSourceOnMode(QStringList() << "m0");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.canChangeRange(), false);
}

void test_cro_channel_can_change_range_observer::canChangeAfterSourceModeIsOnOff()
{
    ChannelPtr channel = std::make_shared<Channel>("m0", netInfo, m_tcpFactory, true);
    channel->startFetch();
    TimeMachineObject::feedEventLoop();
    ChannelCanChangeRangeObserver observer(channel, nullptr);

    QCOMPARE(observer.canChangeRange(), true);

    m_testServer->setSourceOnMode(QStringList() << "m0");
    m_testServer->setSourceOnMode(QStringList());
    TimeMachineObject::feedEventLoop();

    QCOMPARE(observer.canChangeRange(), true);
}

void test_cro_channel_can_change_range_observer::setupServers()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMtxxxs2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

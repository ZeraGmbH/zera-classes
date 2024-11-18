#include "test_pcb_channel_manager.h"
#include "pcbchannelmanager.h"
#include "pcbchannelmanagerformodman.h"
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

QTEST_MAIN(test_pcb_channel_manager)

void test_pcb_channel_manager::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_pcb_channel_manager::init()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpFactory);
}

void test_pcb_channel_manager::cleanup()
{
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
    m_pcbClient = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_pcb_channel_manager::emptyChannelListOnStartup()
{
    PcbChannelManager manager;
    QVERIFY(manager.getChannelMNames().isEmpty());
}

void test_pcb_channel_manager::scanChannelListSignalReturned()
{
    PcbChannelManager manager;
    QSignalSpy spy(&manager, &PcbChannelManager::sigScanFinished);

    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_pcb_channel_manager::scanChannelListChannelsReturned()
{
    PcbChannelManager manager;
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QStringList channels = manager.getChannelMNames();
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

void test_pcb_channel_manager::rescanWithoutClear()
{
    PcbChannelManager manager;
    QSignalSpy spyRangesChanged(&manager, &PcbChannelManager::sigChannelRangesChanged);
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyRangesChanged.count(), 8);
    QStringList channels = manager.getChannelMNames();
    QCOMPARE(channels.count(), 8);

    spyRangesChanged.clear();
    cleanup(); // make sure no I/O is performed by killing servers

    QSignalSpy spyFinished(&manager, &PcbChannelManager::sigScanFinished);
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(manager.getChannelMNames().count(), 8);
    QCOMPARE(spyRangesChanged.count(), 0);
    QCOMPARE(spyFinished.count(), 1);
}

void test_pcb_channel_manager::rescanWithClear()
{
    PcbChannelManagerForModman manager;
    QSignalSpy spyRangesChanged(&manager, &PcbChannelManager::sigChannelRangesChanged);
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyRangesChanged.count(), 8);
    QStringList channels = manager.getChannelMNames();
    QCOMPARE(channels.count(), 8);

    spyRangesChanged.clear();
    manager.clear();
    QCOMPARE(manager.getChannelMNames().count(), 0);

    QSignalSpy spyFinished(&manager, &PcbChannelManager::sigScanFinished);
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(manager.getChannelMNames().count(), 8);
    QCOMPARE(spyRangesChanged.count(), 8);
    QCOMPARE(spyFinished.count(), 1);
}

void test_pcb_channel_manager::checkAlias()
{
    PcbChannelManager manager;
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(manager.getChannelData("m0")->m_alias, "UL1");
    QCOMPARE(manager.getChannelData("m1")->m_alias, "UL2");
    QCOMPARE(manager.getChannelData("m2")->m_alias, "UL3");
    QCOMPARE(manager.getChannelData("m3")->m_alias, "IL1");
    QCOMPARE(manager.getChannelData("m4")->m_alias, "IL2");
    QCOMPARE(manager.getChannelData("m5")->m_alias, "IL3");
    QCOMPARE(manager.getChannelData("m6")->m_alias, "UAUX");
    QCOMPARE(manager.getChannelData("m7")->m_alias, "IAUX");
}

void test_pcb_channel_manager::checkDspChannel()
{
    PcbChannelManager manager;
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(manager.getChannelData("m0")->m_dspChannel, 0);
    QCOMPARE(manager.getChannelData("m1")->m_dspChannel, 2);
    QCOMPARE(manager.getChannelData("m2")->m_dspChannel, 4);
    QCOMPARE(manager.getChannelData("m3")->m_dspChannel, 1);
    QCOMPARE(manager.getChannelData("m4")->m_dspChannel, 3);
    QCOMPARE(manager.getChannelData("m5")->m_dspChannel, 5);
    QCOMPARE(manager.getChannelData("m6")->m_dspChannel, 6);
    QCOMPARE(manager.getChannelData("m7")->m_dspChannel, 7);
}

void test_pcb_channel_manager::checkUnit()
{
    PcbChannelManager manager;
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(manager.getChannelData("m0")->m_unit, "V");
    QCOMPARE(manager.getChannelData("m3")->m_unit, "A");
}

void test_pcb_channel_manager::checkRanges()
{
    PcbChannelManager manager;
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(manager.getChannelRanges("m0").size(), 3);
    QCOMPARE(manager.getChannelRanges("m0")[0], "250V");
}

void test_pcb_channel_manager::changeRangesByClamp()
{
    PcbChannelManager manager;
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();
    QStringList ranges = manager.getChannelRanges("m3");
    QCOMPARE(ranges.size(), 21);

    m_testServer->addClamp(cClamp::CL120A, "IL1");
    TimeMachineObject::feedEventLoop();

    ranges = manager.getChannelRanges("m3");
    QCOMPARE(ranges.size(), 30);

    TimeMachineObject::feedEventLoop();
}

void test_pcb_channel_manager::notifyRangeChangeByClamp()
{
    PcbChannelManager manager;
    QSignalSpy spy(&manager, &PcbChannelManager::sigChannelRangesChanged);
    manager.startScan(m_pcbClient);
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

void test_pcb_channel_manager::getDataForInvalidChannel()
{
    PcbChannelManagerForModman manager;
    manager.startScan(m_pcbClient);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(manager.getChannelData("foo"), nullptr);
}

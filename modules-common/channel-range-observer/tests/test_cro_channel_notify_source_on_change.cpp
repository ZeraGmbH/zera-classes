#include "test_cro_channel_notify_source_on_change.h"
#include "rangemodule.h"
#include <timemachineobject.h>
#include <modulemanagertestrunner.h>
#include <proxy.h>
#include <controllerpersitentdata.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_cro_channel_notify_source_on_change)

void test_cro_channel_notify_source_on_change::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_cro_channel_notify_source_on_change::cleanup()
{
    TimeMachineObject::feedEventLoop();
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
}

constexpr int rangemoduleEntityId = 1020;

void test_cro_channel_notify_source_on_change::mt581s2NotifyManyChannels()
{
    ModuleManagerTestRunner testRunner(":/def-session-minimal.json", false, "mt581s2");
    createClient();

    RANGEMODULE::cRangeModule *rangeModule = qobject_cast<RANGEMODULE::cRangeModule*>(testRunner.getModule("rangemodule", rangemoduleEntityId));
    ChannelRangeObserver::SystemObserverPtr observer = rangeModule->getSharedChannelRangeObserver();
    ChannelRangeObserver::ChannelPtr channelUL1 = observer->getChannel("m0");
    QSignalSpy spylUL1(channelUL1.get(), &ChannelRangeObserver::Channel::sigSourceModeOnChanged);
    ChannelRangeObserver::ChannelPtr channelIL1 = observer->getChannel("m3");
    QSignalSpy spylIL1(channelIL1.get(), &ChannelRangeObserver::Channel::sigSourceModeOnChanged);

    QCOMPARE(observer->hasInternalSourceGenerator(), true);
    QCOMPARE(channelUL1->isSourceModeOn(), false);
    QCOMPARE(channelIL1->isSourceModeOn(), false);

    spylUL1.clear();
    spylIL1.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON m0;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spylUL1.count(), 1);
    QCOMPARE(spylUL1[0][0], true);
    QCOMPARE(spylIL1.count(), 0);
    QCOMPARE(channelUL1->isSourceModeOn(), true);
    QCOMPARE(channelIL1->isSourceModeOn(), false);

    spylUL1.clear();
    spylIL1.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON m0,m3;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spylUL1.count(), 0);
    QCOMPARE(spylIL1.count(), 1);
    QCOMPARE(spylIL1[0][0], true);
    QCOMPARE(channelUL1->isSourceModeOn(), true);
    QCOMPARE(channelIL1->isSourceModeOn(), true);

    spylUL1.clear();
    spylIL1.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spylUL1.count(), 1);
    QCOMPARE(spylIL1.count(), 1);
    QCOMPARE(spylUL1[0][0], false);
    QCOMPARE(spylIL1[0][0], false);
    QCOMPARE(channelUL1->isSourceModeOn(), false);
    QCOMPARE(channelIL1->isSourceModeOn(), false);

    spylUL1.clear();
    spylIL1.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON m0,m3;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spylUL1.count(), 1);
    QCOMPARE(spylIL1.count(), 1);
    QCOMPARE(spylUL1[0][0], true);
    QCOMPARE(spylIL1[0][0], true);
    QCOMPARE(channelUL1->isSourceModeOn(), true);
    QCOMPARE(channelIL1->isSourceModeOn(), true);
}

void test_cro_channel_notify_source_on_change::mt310s2s2Startup()
{
    ModuleManagerTestRunner testRunner(":/def-session-minimal.json", false, "mt310s2");
    createClient();

    RANGEMODULE::cRangeModule *rangeModule = qobject_cast<RANGEMODULE::cRangeModule*>(testRunner.getModule("rangemodule", rangemoduleEntityId));
    ChannelRangeObserver::SystemObserverPtr observer = rangeModule->getSharedChannelRangeObserver();
    ChannelRangeObserver::ChannelPtr channelUL1 = observer->getChannel("m0");
    QSignalSpy spylUL1(channelUL1.get(), &ChannelRangeObserver::Channel::sigSourceModeOnChanged);
    ChannelRangeObserver::ChannelPtr channelIL1 = observer->getChannel("m3");
    QSignalSpy spylIL1(channelIL1.get(), &ChannelRangeObserver::Channel::sigSourceModeOnChanged);

    QCOMPARE(observer->hasInternalSourceGenerator(), false);
    QCOMPARE(channelUL1->isSourceModeOn(), false);
    QCOMPARE(channelIL1->isSourceModeOn(), false);
}

void test_cro_channel_notify_source_on_change::com5003Startup()
{
    ModuleManagerTestRunner testRunner(":/def-session-minimal.json", false, "com5003");
    createClient();

    RANGEMODULE::cRangeModule *rangeModule = qobject_cast<RANGEMODULE::cRangeModule*>(testRunner.getModule("rangemodule", rangemoduleEntityId));
    ChannelRangeObserver::SystemObserverPtr observer = rangeModule->getSharedChannelRangeObserver();
    ChannelRangeObserver::ChannelPtr channelUL1 = observer->getChannel("m0");
    QSignalSpy spylUL1(channelUL1.get(), &ChannelRangeObserver::Channel::sigSourceModeOnChanged);
    ChannelRangeObserver::ChannelPtr channelIL1 = observer->getChannel("m3");
    QSignalSpy spylIL1(channelIL1.get(), &ChannelRangeObserver::Channel::sigSourceModeOnChanged);

    QCOMPARE(observer->hasInternalSourceGenerator(), false);
    QCOMPARE(channelUL1->isSourceModeOn(), false);
    QCOMPARE(channelIL1->isSourceModeOn(), false);
}

void test_cro_channel_notify_source_on_change::createClient()
{
    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, VeinTcp::MockTcpNetworkFactory::create());
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

#include "test_sourceswitchjsoninternal.h"
#include "sourceswitchjsoninternal.h"
#include "jsonfilenames.h"
#include "taskgetinternalsourcecapabilities.h"
#include <mocktcpnetworkfactory.h>
#include <proxyclient.h>
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <proxy.h>
#include <testfactoryi2cctrl.h>
#include <testloghelpers.h>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_sourceswitchjsoninternal)

void test_sourceswitchjsoninternal::initTestCase()
{
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_sourceswitchjsoninternal::init()
{
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpNetworkFactory, "mt581s2d");
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    m_pcbIFace = std::make_shared<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();

    m_capabilities = std::make_shared<QJsonObject>();
    TaskTemplatePtr capabilitiesTask = TaskGetInternalSourceCapabilities::create(
        m_pcbIFace,
        m_capabilities,
        []{}, TRANSACTION_TIMEOUT);
    capabilitiesTask->start();
    TimeMachineObject::feedEventLoop();
}

void test_sourceswitchjsoninternal::cleanup()
{
    m_pcbIFace.reset();
    m_proxyClient.reset();
    m_mt310s2d.reset();
    TimeMachineObject::feedEventLoop();
}

void test_sourceswitchjsoninternal::checkInitialLoadState()
{
    SourceSwitchJsonInternal switcher(m_pcbIFace, *m_capabilities);
    JsonParamApi load = switcher.getCurrLoadState();
    QByteArray dumped = TestLogHelpers::dump(load.getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/loadDumps/initialLoad.json", dumped));
}

void test_sourceswitchjsoninternal::switchOnOk()
{
    SourceSwitchJsonInternal switcher(m_pcbIFace, *m_capabilities);
    QSignalSpy spy(&switcher, &AbstractSourceSwitchJson::sigSwitchFinished);

    JsonParamApi load = switcher.getCurrLoadState();
    load.setOn(true);
    const QByteArray onState = TestLogHelpers::dump(load.getParams());

    switcher.switchState(load);
    QByteArray dumpedRequested = TestLogHelpers::dump(switcher.getRequestedLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(onState, dumpedRequested));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);

    QByteArray dumpedCurrent = TestLogHelpers::dump(switcher.getCurrLoadState().getParams());
    QByteArray expectedCurrent = TestLogHelpers::dump(load.getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(expectedCurrent, dumpedCurrent));
}

void test_sourceswitchjsoninternal::switchOffOk()
{
    SourceSwitchJsonInternal switcher(m_pcbIFace, *m_capabilities);
    QSignalSpy spy(&switcher, &AbstractSourceSwitchJson::sigSwitchFinished);

    switcher.switchOff();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);

    QByteArray dumped = TestLogHelpers::dump(switcher.getCurrLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/loadDumps/initialLoad.json", dumped));
}

void test_sourceswitchjsoninternal::switchOnOffOk()
{
    SourceSwitchJsonInternal switcher(m_pcbIFace, *m_capabilities);
    QSignalSpy spy(&switcher, &AbstractSourceSwitchJson::sigSwitchFinished);
    const QByteArray offState = TestLogHelpers::loadFile(":/loadDumps/initialLoad.json");
    const QByteArray onState = TestLogHelpers::loadFile(":/loadDumps/onLoad.json");

    // On
    JsonParamApi load = switcher.getCurrLoadState();
    load.setOn(true);
    switcher.switchState(load);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(onState, TestLogHelpers::dump(load.getParams())));

    QByteArray dumpedOnRequested = TestLogHelpers::dump(switcher.getRequestedLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(onState, dumpedOnRequested));
    QByteArray dumpedOnCurrent = TestLogHelpers::dump(switcher.getCurrLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(offState, dumpedOnCurrent));

    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    dumpedOnRequested = TestLogHelpers::dump(switcher.getRequestedLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(onState, dumpedOnRequested));
    dumpedOnCurrent = TestLogHelpers::dump(switcher.getCurrLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(onState, dumpedOnCurrent));

    // Off
    switcher.switchOff();
    QByteArray dumpedOffRequested = TestLogHelpers::dump(switcher.getRequestedLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(offState, dumpedOffRequested));
    QByteArray dumpedOffCurrent = TestLogHelpers::dump(switcher.getCurrLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(onState, dumpedOffCurrent));

    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
    dumpedOffRequested = TestLogHelpers::dump(switcher.getRequestedLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(offState, dumpedOffRequested));
    dumpedOffCurrent = TestLogHelpers::dump(switcher.getCurrLoadState().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(offState, dumpedOffCurrent));
}

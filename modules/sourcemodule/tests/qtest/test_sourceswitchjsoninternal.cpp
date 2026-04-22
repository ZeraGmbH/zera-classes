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
        TRANSACTION_TIMEOUT);
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
    switcher.switchState(load);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);

    QByteArray dumped = TestLogHelpers::dump(switcher.getCurrLoadState().getParams());
    QByteArray expected = TestLogHelpers::dump(load.getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(expected, dumped));
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

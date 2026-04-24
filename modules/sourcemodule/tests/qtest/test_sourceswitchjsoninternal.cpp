#include "test_sourceswitchjsoninternal.h"
#include "sourceswitchjsoninternal.h"
#include "taskgeneratorinternalcapabilitiesget.h"
#include "testsourceinternalloadstategetter.h"
#include <mocktcpnetworkfactory.h>
#include <proxyclient.h>
#include <controllerpersitentdata.h>
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <testfloatingpointhelper.h>
#include <proxy.h>
#include <math.h>
#include <testfactoryi2cctrl.h>
#include <testloghelpers.h>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_sourceswitchjsoninternal)

void test_sourceswitchjsoninternal::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_sourceswitchjsoninternal::init()
{
    ControllerPersitentData::cleanupPersitentData();

    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpNetworkFactory, "mt581s2d");
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    m_pcbIFace = std::make_shared<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();

    m_capabilities = std::make_shared<QJsonObject>();
    TaskTemplatePtr capabilitiesTask = TaskGeneratorInternalCapabilitiesGet::create(
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

void test_sourceswitchjsoninternal::checkMockInitialLoadInMock()
{
    // This test just makes sure some assumptions below are valid...
    TestSourceInternalLoadStateGetter getter(m_pcbIFace);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 0), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 1), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 2), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::I, 0), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::I, 1), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::I, 2), false);

    QCOMPARE(getter.getOn(phaseType::U, 0), false);
    QCOMPARE(getter.getOn(phaseType::U, 1), false);
    QCOMPARE(getter.getOn(phaseType::U, 2), false);
    QCOMPARE(getter.getOn(phaseType::I, 0), false);
    QCOMPARE(getter.getOn(phaseType::I, 1), false);
    QCOMPARE(getter.getOn(phaseType::I, 2), false);

    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::U, 0)/M_SQRT2, 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::U, 1)/M_SQRT2, 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::U, 2)/M_SQRT2, 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::I, 0)/M_SQRT2, 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::I, 1)/M_SQRT2, 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::I, 2)/M_SQRT2, 0.0));

    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::U, 0), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::U, 1), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::U, 2), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::I, 0), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::I, 1), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::I, 2), 0.0));

    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::U, 0), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::U, 1), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::U, 2), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::I, 0), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::I, 1), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::I, 2), 0.0));
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
    TestSourceInternalLoadStateGetter getter(m_pcbIFace);


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

    QCOMPARE(getter.getSourceModeOn(phaseType::U, 0), true);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 1), true);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 2), true);
    QCOMPARE(getter.getSourceModeOn(phaseType::I, 0), true);
    QCOMPARE(getter.getSourceModeOn(phaseType::I, 1), true);
    QCOMPARE(getter.getSourceModeOn(phaseType::I, 2), true);

    QCOMPARE(getter.getOn(phaseType::U, 0), true);
    QCOMPARE(getter.getOn(phaseType::U, 1), true);
    QCOMPARE(getter.getOn(phaseType::U, 2), true);
    QCOMPARE(getter.getOn(phaseType::I, 0), true);
    QCOMPARE(getter.getOn(phaseType::I, 1), true);
    QCOMPARE(getter.getOn(phaseType::I, 2), true);

    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::U, 0)/M_SQRT2, 230.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::U, 1)/M_SQRT2, 230.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::U, 2)/M_SQRT2, 230.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::I, 0)/M_SQRT2, 5.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::I, 1)/M_SQRT2, 5.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::I, 2)/M_SQRT2, 5.0));

    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::U, 0), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::U, 1), 120.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::U, 2), 240.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::I, 0), 0.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::I, 1), 120.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::I, 2), 240.0));

    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::U, 0), 50.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::U, 1), 50.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::U, 2), 50.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::I, 0), 50.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::I, 1), 50.0));
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::I, 2), 50.0));


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

    QCOMPARE(getter.getSourceModeOn(phaseType::U, 0), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 1), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 2), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::I, 0), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::I, 1), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::I, 2), false);

    QCOMPARE(getter.getOn(phaseType::U, 0), false);
    QCOMPARE(getter.getOn(phaseType::U, 1), false);
    QCOMPARE(getter.getOn(phaseType::U, 2), false);
    QCOMPARE(getter.getOn(phaseType::I, 0), false);
    QCOMPARE(getter.getOn(phaseType::I, 1), false);
    QCOMPARE(getter.getOn(phaseType::I, 2), false);
}

void test_sourceswitchjsoninternal::switchUL2OffCheckOnFlags()
{
    SourceSwitchJsonInternal switcher(m_pcbIFace, *m_capabilities);

    JsonParamApi load = switcher.getCurrLoadState();
    load.setOn(phaseType::U, 1, false);

    load.setOn(true);
    switcher.switchState(load);
    TimeMachineObject::feedEventLoop();

    TestSourceInternalLoadStateGetter getter(m_pcbIFace);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 0), true);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 1), false);
    QCOMPARE(getter.getSourceModeOn(phaseType::U, 2), true);

    QCOMPARE(getter.getOn(phaseType::U, 0), true);
    QCOMPARE(getter.getOn(phaseType::U, 1), false);
    QCOMPARE(getter.getOn(phaseType::U, 2), true);
}

void test_sourceswitchjsoninternal::switchUL2OffCheckDspAmplitude()
{
    SourceSwitchJsonInternal switcher(m_pcbIFace, *m_capabilities);

    JsonParamApi load = switcher.getCurrLoadState();
    load.setOn(phaseType::U, 1, false);

    load.setRms(phaseType::U, 0, 80.0);
    load.setRms(phaseType::U, 1, 81.0);
    load.setRms(phaseType::U, 2, 82.0);

    load.setOn(true);
    switcher.switchState(load);
    TimeMachineObject::feedEventLoop();

    TestSourceInternalLoadStateGetter getter(m_pcbIFace);
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::U, 0)/M_SQRT2, 80.0));
    // we don't care for Values on UL2
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspPeakAmplitude(phaseType::U, 2)/M_SQRT2, 82.0));
}

void test_sourceswitchjsoninternal::switchUL2OffCheckDspAngle()
{
    SourceSwitchJsonInternal switcher(m_pcbIFace, *m_capabilities);

    JsonParamApi load = switcher.getCurrLoadState();
    load.setOn(phaseType::U, 1, false);

    load.setAngle(phaseType::U, 0, 1.0);
    load.setAngle(phaseType::U, 1, 2.0);
    load.setAngle(phaseType::U, 2, 3.0);

    load.setOn(true);
    switcher.switchState(load);
    TimeMachineObject::feedEventLoop();

    TestSourceInternalLoadStateGetter getter(m_pcbIFace);
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::U, 0), 1.0));
    // we don't care for Values on UL2
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspAngle(phaseType::U, 2), 3.0));
}

void test_sourceswitchjsoninternal::switchUL2OffCheckDspFrequency()
{
    SourceSwitchJsonInternal switcher(m_pcbIFace, *m_capabilities);

    JsonParamApi load = switcher.getCurrLoadState();
    load.setOn(phaseType::U, 1, false);

    load.setFreqVal(51.0);

    load.setOn(true);
    switcher.switchState(load);
    TimeMachineObject::feedEventLoop();

    TestSourceInternalLoadStateGetter getter(m_pcbIFace);
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::U, 0), 51.0));
    // we don't care for Values on UL2
    QVERIFY(TestFloatingPointHelper::compareFuzzyLimit(getter.getDspFreqVal(phaseType::U, 2), 51.0));
}

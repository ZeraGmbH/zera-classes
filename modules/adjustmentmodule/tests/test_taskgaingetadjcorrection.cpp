#include "test_taskgaingetadjcorrection.h"
#include "taskgaingetadjcorrection.h"
#include <proxy.h>
#include <pcbinitfortest.h>
#include <testfactoryi2cctrl.h>
#include <mockeepromdevice.h>
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <mocktcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_taskgaingetadjcorrection);

void test_taskgaingetadjcorrection::initTestCase()
{
    setupServers();

    QString filenameShort = ":/import_internal";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));

    m_valueFormatter = AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter();
}

void test_taskgaingetadjcorrection::cleanup()
{
    TimeMachineObject::feedEventLoop();
    MockEepromDevice::cleanAll();
}

static constexpr double val = 42.0;

void test_taskgaingetadjcorrection::getInternalCorrection()
{
    std::shared_ptr<double> correctionValue = std::make_shared<double>();
    TaskTemplatePtr task = TaskGainGetAdjCorrection::create(m_pcbIFace,
                                                            "m0", "250V", val,
                                                            correctionValue,
                                                            DEFAULT_EXPIRE);
    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(*correctionValue, m_testServer->calcGainValue(val));
}

void test_taskgaingetadjcorrection::getClampCorrection()
{
    m_testServer->addStdClamp();
    std::shared_ptr<double> correctionValue = std::make_shared<double>();
    TaskTemplatePtr task = TaskGainGetAdjCorrection::create(m_pcbIFace,
                                                            "m3", "C200A", val,
                                                            correctionValue,
                                                            DEFAULT_EXPIRE);
    QSignalSpy spy(task.get(), &TaskTemplate::sigFinish);
    task->start();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(*correctionValue, m_testServer->calcGainValueClamp(val));
}

void test_taskgaingetadjcorrection::timeoutAndErrFunc()
{
    std::shared_ptr<double> correctionValue = std::make_shared<double>();
    int localErrorCount = 0;
    PcbInitForTest pcb;
    TaskTemplatePtr task = TaskGainGetAdjCorrection::create(pcb.getPcbInterface(),
                                                            "m3", "C200A", val,
                                                            correctionValue,
                                                            DEFAULT_EXPIRE,
                                                            [&]{
                                                                localErrorCount++;
                                                            });
    TaskTestHelper helper(task.get());
    task->start();
    TimeMachineForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_taskgaingetadjcorrection::setupServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(
        std::make_shared<TestFactoryI2cCtrl>(true),
        tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_shared<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

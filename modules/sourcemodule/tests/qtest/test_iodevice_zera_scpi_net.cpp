#include "test_iodevice_zera_scpi_net.h"
#include "iodevicezerascpinet.h"
#include <testallservicesmt310s2.h>
#include <mocktcpnetworkfactory.h>
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <proxy.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_iodevice_zera_scpi_net)

void test_iodevice_zera_scpi_net::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    TimeMachineForTest::reset();
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
}

void test_iodevice_zera_scpi_net::init()
{
    createRunningServices();
    establishPcbInterface();
}

void test_iodevice_zera_scpi_net::cleanup()
{
    m_pcbIFace.reset();
    m_proxyClient.reset();
    killServices();
}

void test_iodevice_zera_scpi_net::queryCapabilities()
{
    IoDeviceZeraSCPINet ioDevice(m_pcbIFace);
    QSignalSpy spy(&ioDevice, &IoDeviceBase::sigIoFinished);
    ioDevice.open("foo");

    IoTransferDataSingle::Ptr transfer = IoTransferDataSingle::Ptr::create("GENERATOR:CAPABILITIES?", "", "");
    int id = ioDevice.sendAndReceive(transfer);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], id);
    QCOMPARE(spy[0][1], false);

    QJsonObject jsonCaps = QJsonDocument::fromJson(transfer->getBytesReceived()).object();
    QCOMPARE(jsonCaps["Name"], "MT581s2");
    QCOMPARE(transfer->wasNotRunYet(), false);
    QCOMPARE(transfer->wrongAnswerReceived(), false);
    QCOMPARE(transfer->didIoPass(), true);
    QCOMPARE(transfer->noAnswerReceived(), false);
    QCOMPARE(transfer->queryContentFailed(), false);

    TimeMachineForTest::getInstance()->processTimers(TRANSACTION_TIMEOUT);
    QCOMPARE(spy.count(), 1);
}

void test_iodevice_zera_scpi_net::queryCapabilitiesWrongAnswer()
{
    IoDeviceZeraSCPINet ioDevice(m_pcbIFace);
    QSignalSpy spy(&ioDevice, &IoDeviceBase::sigIoFinished);
    ioDevice.open("foo");

    IoTransferDataSingle::Ptr transfer = IoTransferDataSingle::Ptr::create("GENERATOR:CAPABILITIES?", "", "wrongTrailing");
    int id = ioDevice.sendAndReceive(transfer);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], id);
    QCOMPARE(spy[0][1], false);

    QJsonObject jsonCaps = QJsonDocument::fromJson(transfer->getBytesReceived()).object();
    QCOMPARE(jsonCaps["Name"], "MT581s2");
    QCOMPARE(transfer->wrongAnswerReceived(), true);
    QCOMPARE(transfer->wasNotRunYet(), false);
    QCOMPARE(transfer->didIoPass(), false);
    QCOMPARE(transfer->noAnswerReceived(), false);
    QCOMPARE(transfer->queryContentFailed(), false);
}

void test_iodevice_zera_scpi_net::queryBullshit()
{
    IoDeviceZeraSCPINet ioDevice(m_pcbIFace);
    QSignalSpy spy(&ioDevice, &IoDeviceBase::sigIoFinished);
    ioDevice.open("foo");

    IoTransferDataSingle::Ptr transfer = IoTransferDataSingle::Ptr::create("foo?", "", "");
    int id = ioDevice.sendAndReceive(transfer);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], id);
    QCOMPARE(spy[0][1], true);

    QCOMPARE(transfer->wrongAnswerReceived(), true);
    QCOMPARE(transfer->wasNotRunYet(), false);
    QCOMPARE(transfer->didIoPass(), false);
    QCOMPARE(transfer->noAnswerReceived(), false);
    QCOMPARE(transfer->queryContentFailed(), false);
}

void test_iodevice_zera_scpi_net::queryNoServer()
{
    killServices();
    IoDeviceZeraSCPINet ioDevice(m_pcbIFace);
    QSignalSpy spy(&ioDevice, &IoDeviceBase::sigIoFinished);
    ioDevice.open("foo");

    IoTransferDataSingle::Ptr transfer = IoTransferDataSingle::Ptr::create("GENERATOR:CAPABILITIES?", "", "", TRANSACTION_TIMEOUT);
    int id = ioDevice.sendAndReceive(transfer);
    TimeMachineForTest::getInstance()->processTimers(TRANSACTION_TIMEOUT/2);
    QCOMPARE(spy.count(), 0);

    TimeMachineForTest::getInstance()->processTimers(TRANSACTION_TIMEOUT/2);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], id);
    QCOMPARE(spy[0][1], true);

    QCOMPARE(transfer->didIoPass(), false);
    QCOMPARE(transfer->noAnswerReceived(), true);
}

void test_iodevice_zera_scpi_net::createRunningServices()
{
    m_testAllServices = std::make_unique<TestAllServicesMt310s2>(
        "mt581s2",
        m_tcpNetworkFactory,
        std::make_shared<TestFactoryI2cCtrl>(false));
}

void test_iodevice_zera_scpi_net::killServices()
{
    m_testAllServices.reset();
    TimeMachineObject::feedEventLoop();
}

void test_iodevice_zera_scpi_net::establishPcbInterface()
{
    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    m_pcbIFace = std::make_shared<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

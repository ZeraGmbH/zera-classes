#ifndef TEST_IODEVICE_ZERA_SCPI_NET_H
#define TEST_IODEVICE_ZERA_SCPI_NET_H

#include <QObject>
#include <abstracttestallservices.h>
#include <pcbinterface.h>
#include <proxyclient.h>

class test_iodevice_zera_scpi_net : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void queryCapabilities();
    void queryCapabilitiesWrongAnswer();
    void queryBullshit();
    void queryNoServer();

private:
    void createRunningServices();
    void killServices();
    void establishPcbInterface();

    std::unique_ptr<AbstractTestAllServices> m_testAllServices;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    Zera::ProxyClientPtr m_proxyClient;
    Zera::PcbInterfacePtr m_pcbIFace;
};

#endif // TEST_IODEVICE_ZERA_SCPI_NET_H

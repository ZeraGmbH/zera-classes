#ifndef TEST_CRO_CHANNELCURRENTRANGE_H
#define TEST_CRO_CHANNELCURRENTRANGE_H

#include <pcbinterface.h>
#include <abstracttcpnetworkfactory.h>
#include <resmanrunfacade.h>
#include <testserverforsenseinterfacemt310s2.h>

class test_cro_channelcurrentrange : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void noRangeOnStartup();
    void signalsForValidFetch();
    void signalsForInvalidPort();
    void signalsForInvalidChannel();
    void getForValidFetch();
    void handleRangeChange();
    void afterRangeChange();
private:
    void setupServers();
    void setupClient();
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;

    Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;
};

#endif // TEST_CRO_CHANNELCURRENTRANGE_H

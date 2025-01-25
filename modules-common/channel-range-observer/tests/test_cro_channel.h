#ifndef TEST_CRO_CHANNEL_H
#define TEST_CRO_CHANNEL_H

#include <pcbinterface.h>
#include <abstracttcpnetworkfactory.h>
#include <resmanrunfacade.h>
#include <testserverforsenseinterfacemt310s2.h>

class test_cro_channel : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void emptyOnStartup();
    void fetchDirect();
    void fetchDirectTwice();
    void fetchInvalidChannel();
    void fetchByTaskValid();
    void fetchByTaskInvalid();
    void fetchCheckChannelDataM0();
    void fetchCheckChannelDataM3();
    void refetchAlthoughNotSuggestedWorks();
    void getRangesCheckBasicData();
    void checkUrValue();
    void checkRangeAvailable();
    void checkOrderingVoltageRanges();
    void checkOrderingAllCurrentRanges();
    void checkAvailableRangesMtDefaultAc();
    void checkAvailableRangesMtAdj();
    void checkAvailableRangesMtSequence();
    void checkScanTwiceAvailableRangesMtAdj();

    void notifyRangeChangeByClampComeAndGo();
private:
    void setupServers();
    void setupClient();
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;

    Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;
};

#endif // TEST_CRO_CHANNEL_H

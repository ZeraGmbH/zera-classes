#ifndef TEST_CHANNEL_OBSERVER_H
#define TEST_CHANNEL_OBSERVER_H

#include <abstracttcpnetworkfactory.h>
#include <resmanrunfacade.h>
#include <testserverforsenseinterfacemt310s2.h>

class test_channel_observer : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();
    void emptyOnStartup();
    void fetchDirect();
    void fetchDirectTwice();
    void fetchByTask();
    void fetchCheckChannelDataM0();
    void fetchCheckChannelDataM3();
    void getRangesCheckBasicData();
    void checkUrValue();
    void checkOrderingVoltageRanges();
    void checkOrderingAllCurrentRanges();
private:
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
};

#endif // TEST_CHANNEL_OBSERVER_H

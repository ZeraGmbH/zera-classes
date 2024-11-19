#ifndef TEST_CHANNEL_RANGE_OBSERVER_H
#define TEST_CHANNEL_RANGE_OBSERVER_H

#include <testfactoryi2cctrl.h>
#include <abstracttcpnetworkfactory.h>
#include <resmanrunfacade.h>
#include <proxyclient.h>
#include <testserverforsenseinterfacemt310s2.h>
#include <QObject>

class test_channel_range_observer : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();
    void emptyChannelListOnStartup();
    void scanChannelListSignalReturned();
    void scanChannelListChannelsReturned();
    void rescanWithoutClear();
    void rescanWithClear();
    void checkAlias();
    void checkDspChannel();
    void checkUnit();
    void checkRanges();
    void changeRangesByClamp();
    void notifyRangeChangeByClamp();
    void getDataForInvalidChannel();
private:
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
};

#endif // TEST_CHANNEL_RANGE_OBSERVER_H

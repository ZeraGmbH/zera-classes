#ifndef TEST_PCB_CHANNEL_MANAGER_H
#define TEST_PCB_CHANNEL_MANAGER_H

#include <testfactoryi2cctrl.h>
#include <abstracttcpnetworkfactory.h>
#include <resmanrunfacade.h>
#include <proxyclient.h>
#include <testserverforsenseinterfacemt310s2.h>
#include <QObject>

class test_pcb_channel_manager : public QObject
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
    void getDataForInvalidChannel();
private:
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
};

#endif // TEST_PCB_CHANNEL_MANAGER_H

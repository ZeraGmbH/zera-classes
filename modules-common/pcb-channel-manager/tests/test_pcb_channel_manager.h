#ifndef TEST_PCB_CHANNEL_MANAGER_H
#define TEST_PCB_CHANNEL_MANAGER_H

#include <testfactoryi2cctrl.h>
#include <abstracttcpnetworkfactory.h>
#include <abstractmockallservices.h>
#include <proxyclient.h>
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
    void getDataForInvalidChannel();
private:
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::shared_ptr<TestFactoryI2cCtrl> m_i2cFactory;
    std::unique_ptr<AbstractMockAllServices> m_allServices;
    Zera::ProxyClientPtr m_pcbClient;
};

#endif // TEST_PCB_CHANNEL_MANAGER_H

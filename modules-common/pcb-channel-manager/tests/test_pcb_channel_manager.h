#ifndef TEST_PCB_CHANNEL_MANAGER_H
#define TEST_PCB_CHANNEL_MANAGER_H

#include <testfactoryi2cctrl.h>
#include <abstracttcpnetworkfactory.h>
#include <QObject>

class test_pcb_channel_manager : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void emptyChannelListOnStartup();
    void scanChannelListSignalReturned();
    void scanChannelListChannelsReturned();
private:
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::shared_ptr<TestFactoryI2cCtrl> m_i2cFactory;
};

#endif // TEST_PCB_CHANNEL_MANAGER_H

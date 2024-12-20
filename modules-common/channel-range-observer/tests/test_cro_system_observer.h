#ifndef TEST_CRO_ALL_CHANNELS_H
#define TEST_CRO_ALL_CHANNELS_H

#include <abstracttcpnetworkfactory.h>
#include <resmanrunfacade.h>
#include <testserverforsenseinterfacemt310s2.h>

class test_cro_system_observer : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();
    void emptyChannelListOnStartup();
    void scanChannelListSignalReturned();
    void scanChannelListChannelsReturned();
    void scanByTaskValid();
    void scanByTaskInvalid();
    void rescanWithoutClear();
    void rescanWithClear();
    void checkSampleRate();
    void checkValidChannels();
    void checkAlias();
    void checkDspChannel();
    void checkUnit();
    void checkRanges();
    void changeRangesByClamp();
    void notifyRangeChangeByClamp();
    void getDataForInvalidChannel();
    void invalidScan();
private:
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
};

#endif // TEST_CRO_ALL_CHANNELS_H

#ifndef test_all_channels_observer_H
#define test_all_channels_observer_H

#include <abstracttcpnetworkfactory.h>
#include <resmanrunfacade.h>
#include <testserverforsenseinterfacemt310s2.h>

class test_all_channels_observer : public QObject
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
};

#endif // test_all_channels_observer_H

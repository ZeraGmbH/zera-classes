#ifndef TEST_CRO_RANGE_H
#define TEST_CRO_RANGE_H

#include <pcbinterface.h>
#include <proxyclient.h>
#include <resmanrunfacade.h>
#include <testserverforsenseinterfacemt310s2.h>
#include <abstracttcpnetworkfactory.h>
#include <QObject>

class test_cro_range : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void defaultOnStartup();
    void fetchAvailable();
    void fetchNotAvailable();
    void fetchInvalidChannel();
    void fetchInvalidRange();
    void fetchTwice();
    void fetchByTaskValid();
    void fetchByTaskInvalid();
    void refetchAlthoughNotSuggestedWorks();
    void checkUrValue();
    void checkType();
    void checkRejection();
    void checkOvRejection();
    void checkAdcRejection();
private:
    void setupServers();
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;

    Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;
};

#endif // TEST_CRO_RANGE_H

#ifndef TEST_TASKOFFSETGETADJCORRECTION_H
#define TEST_TASKOFFSETGETADJCORRECTION_H

#include <pcbinterface.h>
#include <testserverforsenseinterfacemt310s2.h>
#include <proxyclient.h>
#include <resmanrunfacade.h>

class test_taskoffsetgetadjcorrection : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void getInternalCorrection();
    void getClampCorrection();
    void timeoutAndErrFunc();

private:
    void setupServers();

    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;
    std::shared_ptr<Zera::cPCBInterface> m_pcbIFace;
    std::unique_ptr<AdjustScpiValueFormatter> m_valueFormatter;
};

#endif // TEST_TASKOFFSETGETADJCORRECTION_H

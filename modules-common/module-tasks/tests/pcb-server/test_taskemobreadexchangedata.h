#ifndef TEST_TASKEMOBREADEXCHANGEDATA_H
#define TEST_TASKEMOBREADEXCHANGEDATA_H

#include <mockmt310s2d.h>
#include <pcbinterface.h>
#include <proxyclient.h>
#include <resmanrunfacade.h>

class test_taskemobreadexchangedata : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void timeoutAndErrFunc();
    void readProperly();
    void readNoEmob();
private:
    void setupServers();

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::shared_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_TASKEMOBREADEXCHANGEDATA_H

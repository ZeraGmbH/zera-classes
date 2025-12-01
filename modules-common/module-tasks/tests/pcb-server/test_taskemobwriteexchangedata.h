#ifndef TEST_TASKEMOBWRITEEXCHANGEDATA_H
#define TEST_TASKEMOBWRITEEXCHANGEDATA_H

#include <mockmt310s2d.h>
#include <pcbinterface.h>
#include <proxyclient.h>
#include <resmanrunfacade.h>

class test_taskemobwriteexchangedata : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void timeoutAndErrFunc();
    void writeProperly();
    void writeWrongChannel();
    void writeNoEmob();
private:
    void setupServers();
    QByteArray createTestEmobExchangeData();

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::shared_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_TASKEMOBWRITEEXCHANGEDATA_H

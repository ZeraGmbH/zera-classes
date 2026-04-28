#ifndef TEST_SOURCESWITCHJSONINTERNAL_H
#define TEST_SOURCESWITCHJSONINTERNAL_H

#include <pcbinterface.h>
#include <mockmt310s2d.h>
#include <resmanrunfacade.h>

class test_sourceswitchjsoninternal : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void checkInitialLoadState();
    void checkMockInitialLoadInMock();
    void switchOnOk();
    void switchOffOk();
    void switchOnOffOk();
    void switchOnServerDied();
    void twoSignalsSwitchSameTwice();
    void twoSwitchDifferent();

    void switchUL2OffCheckOnFlags();
    void switchUL2OffCheckDspAmplitude();
    void switchUL2OffCheckDspAngle();
    void switchUL2OffCheckDspFrequency();

private:
    void killServer();

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;

    Zera::ProxyClientPtr m_proxyClient;
    std::shared_ptr<Zera::cPCBInterface> m_pcbIFace;

    std::shared_ptr<QJsonObject> m_capabilities;
};

#endif // TEST_SOURCESWITCHJSONINTERNAL_H

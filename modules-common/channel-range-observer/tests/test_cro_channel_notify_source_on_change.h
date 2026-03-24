#ifndef TEST_CRO_CHANNEL_NOTIFY_SOURCE_ON_CHANGE_H
#define TEST_CRO_CHANNEL_NOTIFY_SOURCE_ON_CHANGE_H

#include <QObject>
#include <pcbinterface.h>
#include <proxyclient.h>

class test_cro_channel_notify_source_on_change : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void mt581s2NotifyManyChannels();
    void mt310s2s2Startup();
    void com5003Startup();

private:
    void createClient();
    Zera::ProxyClientPtr m_proxyClient;
    Zera::PcbInterfacePtr m_pcbIFace;
};

#endif // TEST_CRO_CHANNEL_NOTIFY_SOURCE_ON_CHANGE_H

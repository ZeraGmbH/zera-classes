#ifndef CRO_CHANNELCURRENTRANGE_H
#define CRO_CHANNELCURRENTRANGE_H

#include <QObject>
#include <networkconnectioninfo.h>
#include <abstracttcpnetworkfactory.h>
#include <pcbinterface.h>
#include <taskcontainerinterface.h>

namespace ChannelRangeObserver {

class ChannelCurrentRange : public QObject
{
    Q_OBJECT
public:
    ChannelCurrentRange(const QString &channelMName,
                        const NetworkConnectionInfo &netInfo,
                        VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startObserve();
    const QString getCurrentRange() const;
signals:
    void sigFetchDoneCurrentRange(const QString &channelMName, const QString &rangeName, bool ok);

private slots:
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, const QVariant &answer);
private:
    void preparePcbInterface();
    void startTasks();
    TaskTemplatePtr getPcbConnectionTask();
    static void notifyError(const QString &errMsg);

    const QString m_channelMName;
    std::shared_ptr<QString> m_currentRangeName = std::make_shared<QString>();

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    const Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
};

}
#endif // CRO_CHANNELCURRENTRANGE_H

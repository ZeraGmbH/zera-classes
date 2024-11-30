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
    void sigFetchComplete(QString channelMName, QString rangeName, bool ok);

private slots:
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void preparePcbInterface();
    void startTasks();
    TaskTemplatePtr getPcbConnectionTask();
    static void notifyError(QString errMsg);

    const QString m_channelMName;
    QString m_currentRangeName;

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    const Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
};

}
#endif // CRO_CHANNELCURRENTRANGE_H

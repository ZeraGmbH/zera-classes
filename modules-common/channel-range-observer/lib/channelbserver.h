#ifndef RANGEOBSERVER_H
#define RANGEOBSERVER_H

#include <networkconnectioninfo.h>
#include <abstracttcpnetworkfactory.h>
#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <QObject>
#include <QStringList>
#include <QHash>
#include <memory>

class ChannelObserver : public QObject
{
    Q_OBJECT
public:
    QString m_alias;
    QString m_unit;
    int m_dspChannel;

    ChannelObserver(const QString &channelMName,
                    const NetworkConnectionInfo &netInfo,
                    VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startFetch();
    const QStringList getRangeNames() const;
    struct RangeData
    {
        double m_urValue;
    };
    const std::shared_ptr<RangeData> getRangeData(const QString &rangeName) const;
signals:
    void sigFetchComplete(QString channelMName); // TODO change argument to bool ok

private slots:
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    TaskTemplatePtr getPcbConnectionTask();
    TaskTemplatePtr getChannelReadDetailsTask();
    TaskTemplatePtr getAllRangesTask();
    TaskTemplatePtr getRangesRegisterChangeNotificationTask();
    TaskTemplatePtr getReadRangeFinalTask();
    static void notifyError(QString errMsg);

    QString m_channelMName;
    QHash<QString, std::shared_ptr<RangeData>> m_rangeNameToRangeData;

    Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;

    TaskContainerInterfacePtr m_currentTasks;
    QStringList m_rangesNamesOrderedByServer; // maybe when we ordering as servers suggest we use m_rangesNamesOrderedByServer
};

typedef std::shared_ptr<ChannelObserver> ChannelObserverPtr;

#endif // RANGEOBSERVER_H

#ifndef RANGEOBSERVER_H
#define RANGEOBSERVER_H

#include <networkconnectioninfo.h>
#include <abstracttcpnetworkfactory.h>
#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <QObject>
#include <QStringList>
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
    const QStringList getRangeNames() const;
    void startFetch();
signals:
    void sigFetchComplete(QString channelMName); // TODO change argument to bool ok

private slots:
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    TaskTemplatePtr getPcbConnectionTask();
    TaskTemplatePtr getChannelReadDetailsTask();
    TaskTemplatePtr getReadRangeNamesTask();
    TaskTemplatePtr getRangesFetchTasks();
    TaskTemplatePtr getReadRangeDetailsTask();
    TaskTemplatePtr getReadRangeFinalTask();
    static void notifyError(QString errMsg);

    QString m_channelMName;

    QStringList m_tempRangesNames;
    TaskContainerInterfacePtr m_currentTasks;

    Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
};

typedef std::shared_ptr<ChannelObserver> ChannelObserverPtr;

#endif // RANGEOBSERVER_H

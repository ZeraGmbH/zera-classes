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
    // TODO: What if we are asked while fetching?
    const QStringList getAllRangeNames() const;
    const QStringList getAvailRangeNames() const;
    struct RangeData
    {
        bool m_available = false;
        double m_urValue = 0.0;
    };
    const std::shared_ptr<RangeData> getRangeData(const QString &rangeName) const;
signals:
    void sigFetchComplete(QString channelMName);

private slots:
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void clearRanges();
    TaskTemplatePtr getPcbConnectionTask();
    TaskTemplatePtr getChannelReadDetailsTask();
    TaskTemplatePtr getAllRangesTask();
    TaskContainerInterfacePtr addRangeDataTasks(TaskContainerInterfacePtr taskContainer,
                                                const QString &rangeName, std::shared_ptr<RangeData> newRange);
    TaskTemplatePtr getRangesRegisterChangeNotificationTask();
    TaskTemplatePtr getReadRangeFinalTask();
    void setAvailableRanges();
    static void notifyError(QString errMsg);

    const QString m_channelMName;
    QStringList m_allRangeNamesOrderedByServer;
    QStringList m_availableRangeNames;
    QHash<QString, std::shared_ptr<RangeData>> m_rangeNameToRangeData;

    const Zera::ProxyClientPtr m_pcbClient;
    const Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
};

typedef std::shared_ptr<ChannelObserver> ChannelObserverPtr;

#endif // RANGEOBSERVER_H

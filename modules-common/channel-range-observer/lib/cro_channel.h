#ifndef CRO_CHANNEL_H
#define CRO_CHANNEL_H

#include "cro_range.h"
#include <networkconnectioninfo.h>
#include <abstracttcpnetworkfactory.h>
#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <QObject>
#include <QStringList>
#include <QHash>
#include <memory>

namespace ChannelRangeObserver {

class Channel : public QObject
{
    Q_OBJECT
public:
    QString m_alias;
    QString m_unit;
    int m_dspChannel;

    Channel(const QString &channelMName,
            const NetworkConnectionInfo &netInfo,
            VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startFetch();
    // TODO: What if we are asked while fetching?
    const QStringList getAllRangeNames() const;
    const QStringList getAvailRangeNames() const;
    const RangePtr getRange(const QString &rangeName) const;
signals:
    void sigFetchComplete(QString channelMName, bool ok);

private slots:
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void clearRanges();
    void startAllRangesTasks();
    TaskTemplatePtr getPcbConnectionTask();
    TaskTemplatePtr getChannelReadDetailsTask();
    TaskTemplatePtr getRangesRegisterChangeNotificationTask();
    TaskTemplatePtr getFetchFinalTask();
    void setAvailableRanges();
    static void notifyError(QString errMsg);

    const QString m_channelMName;
    QStringList m_allRangeNamesOrderedByServer;
    QStringList m_availableRangeNames;
    QHash<QString, RangePtr> m_rangeNameToRange;

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    const Zera::ProxyClientPtr m_pcbClient;
    const Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
};

typedef std::shared_ptr<Channel> ChannelPtr;

}

#endif // CRO_CHANNEL_H

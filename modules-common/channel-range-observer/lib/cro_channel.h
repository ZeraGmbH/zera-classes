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
    Channel(const QString &channelMName,
            const NetworkConnectionInfo &netInfo,
            VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    // TODO: What if we are asked while fetching?
    void startFetch();
    const QString getMName() const;
    bool isValid() const;
    QString getAlias() const;
    QString getUnit() const;
    int getDspChannel() const;
    const QStringList getAllRangeNames() const;
    const QStringList getAvailRangeNames() const;
    const RangePtr getRange(const QString &rangeName) const;
signals:
    void sigRangeChangeReported(QString channelMName, int interruptCounter);
    void sigFetchDoneChannel(QString channelMName, bool ok);

private slots:
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void clearRanges();
    void preparePcbInterface();
    void startAllRangesTasks();
    TaskTemplatePtr getPcbConnectionTask();
    TaskTemplatePtr getChannelReadDetailsTask();
    TaskTemplatePtr getRangesRegisterChangeNotificationTask();
    TaskTemplatePtr getFetchFinalTask();
    void setAvailableRanges();
    void notifyRangeChangeReported();
    static void notifyError(const QString &errMsg);

    const QString m_channelMName;
    bool m_valid = false;
    QString m_alias;
    std::shared_ptr<QString> m_unit = std::make_shared<QString>();
    int m_dspChannel = 0;

    std::shared_ptr<QStringList> m_allRangeNamesOrderedByServer = std::make_shared<QStringList>();
    QStringList m_availableRangeNames;
    QHash<QString, RangePtr> m_rangeNameToRange;
    int m_rangeChangeInterruptCount = 0;

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    const Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
};

typedef std::shared_ptr<Channel> ChannelPtr;

}

#endif // CRO_CHANNEL_H

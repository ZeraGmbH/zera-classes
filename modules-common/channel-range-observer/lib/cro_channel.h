#ifndef CRO_CHANNEL_H
#define CRO_CHANNEL_H

#include "channelcommonstorage.h"
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
            const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpFactory,
            bool hasInternalSourceGenerator);
    void startFetch();
    const QString &getMName() const;
    const QString &getAlias() const;
    const QString &getUnit() const;
    int getDspChannel() const;
    bool isSourceModeOn() const;
    const QStringList &getAllRangeNames() const;
    const QStringList &getAvailRangeNames() const;
    const RangePtr getRange(const QString &rangeName) const;
    ChannelCommonStorage *getModuleCommonStorage();
signals:
    void sigFetchDoneChannel(const QString &channelMName, bool ok);
    void sigSourceModeOnChanged();

private slots:
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, const QVariant &answer);
private:
    void preparePcbInterface();
    void startAllRangesTasks();
    void doAfterTaskJobs(bool ok);
    TaskTemplatePtr getPcbConnectionTask();
    TaskTemplatePtr getChannelReadDetailsTask();
    TaskTemplatePtr getRangesRegisterChangeNotificationTask();
    static void notifyError(const QString &errMsg);

    const QString m_channelMName;
    std::shared_ptr<QString> m_alias = std::make_shared<QString>();
    std::shared_ptr<QString> m_unit = std::make_shared<QString>();
    std::shared_ptr<int> m_dspChannel = std::make_shared<int>(0);
    bool m_hasInternalSourceGenerator = false;
    bool m_sourceModeOn = false;
    ChannelCommonStorage m_channelAdjStorage;

    struct TRangeData {
        std::shared_ptr<QStringList> m_allRangeNamesOrderedByServer = std::make_shared<QStringList>();
        QStringList m_availableRangeNames;
        QHash<QString, RangePtr> m_rangeNameToRange;
        void clear() {
            m_allRangeNamesOrderedByServer = std::make_shared<QStringList>();
            m_availableRangeNames.clear();
            m_rangeNameToRange.clear();
        };
    };
    TRangeData m_rangesCurr;
    TRangeData m_rangesNext;

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
};

typedef std::shared_ptr<Channel> ChannelPtr;

}

#endif // CRO_CHANNEL_H

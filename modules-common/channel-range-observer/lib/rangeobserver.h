#ifndef RANGEOBSERVER_H
#define RANGEOBSERVER_H

#include <networkconnectioninfo.h>
#include <abstracttcpnetworkfactory.h>
#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <QObject>
#include <QStringList>
#include <memory>

class RangeObserver : public QObject
{
    Q_OBJECT
public:
    QString m_alias;
    QString m_unit;
    int m_dspChannel;

    RangeObserver(const QString &channelMName,
                  const NetworkConnectionInfo &netInfo,
                  VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    const QStringList getRangeNames() const;
    TaskContainerInterfacePtr addRangesFetchTasks(TaskContainerInterfacePtr tasks); // -> rework & private
    void startReadRanges();
signals:
    void sigRangeListChanged(QString channelMName);

private slots:
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    TaskTemplatePtr getReadRangeNamesTask();
    TaskTemplatePtr getReadRangeDetailsTasks();
    TaskTemplatePtr getReadRangeFinalTask();
    static void notifyError(QString errMsg);

    QString m_channelMName;

    QStringList m_tempRangesNames;
    TaskContainerInterfacePtr m_currentTasks;

    Zera::PcbInterfacePtr m_pcbInterface;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
};

typedef std::shared_ptr<RangeObserver> RangeObserverPtr;

#endif // RANGEOBSERVER_H

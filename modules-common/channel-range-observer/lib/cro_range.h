#ifndef CRO_RANGE_H
#define CRO_RANGE_H

#include <taskcontainerinterface.h>
#include <abstracttcpnetworkfactory.h>
#include <networkconnectioninfo.h>
#include <pcbinterface.h>
#include <tasktemplate.h>

namespace ChannelRangeObserver {

class Range : public QObject
{
    Q_OBJECT
public:
    bool m_available = false;
    double m_urValue = 0.0;

    Range(const QString &channelMName, const QString &rangeName,
          const NetworkConnectionInfo &netInfo,
          VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startFetch();
signals:
    void sigFetchComplete(QString channelMName, QString rangeName);

private:
    TaskTemplatePtr getPcbConnectionTask();
    TaskTemplatePtr getFetchFinalTask();
    static void notifyError(QString errMsg);

    const QString m_channelMName;
    const QString m_rangeName;

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    const Zera::ProxyClientPtr m_pcbClient;
    const Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
};

}

#endif // CRO_RANGE_H

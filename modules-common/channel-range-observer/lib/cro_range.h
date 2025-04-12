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
    int m_type = 0;
    double m_urValue = 0.0;
    std::shared_ptr<double> m_rejection = std::make_shared<double>(0.0);;
    std::shared_ptr<double> m_ovrejection = std::make_shared<double>(0.0);
    std::shared_ptr<double> m_adcrejection = std::make_shared<double>(0.0);

    Range(const QString &channelMName, const QString &rangeName,
          const NetworkConnectionInfo &netInfo,
          VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startFetch();
signals:
    void sigFetchDoneRange(QString channelMName, QString rangeName, bool ok);

private:
    void preparePcbInterface();
    TaskTemplatePtr getPcbConnectionTask();
    void notifyError(QString errMsg);

    const QString m_channelMName;
    const QString m_rangeName;

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    const Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
};

typedef std::shared_ptr<Range> RangePtr;

}

#endif // CRO_RANGE_H

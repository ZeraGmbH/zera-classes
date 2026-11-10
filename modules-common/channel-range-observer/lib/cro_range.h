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

    Range(const QString &channelMName, const QString &rangeName,
          const NetworkConnectionInfo &netInfo,
          const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpFactory);

    void startFetch();

    bool getAvailable() const;
    int getType() const;
    const QString &getAlias() const;
    double getUrValue() const;
    double getRejection() const;
    double getOvrRejection() const;
    double getAdcRejection() const;
signals:
    void sigFetchDoneRange(const QString &channelMName, const QString &rangeName, bool ok);

private:
    void preparePcbInterface();
    TaskTemplatePtr getPcbConnectionTask();
    void notifyError(const QString &errMsg);

    const QString m_channelMName;
    const QString m_rangeName;

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    Zera::PcbInterfacePtr m_pcbInterface;

    std::shared_ptr<bool> m_available = std::make_shared<bool>(false);
    std::shared_ptr<int> m_type = std::make_shared<int>(0);
    std::shared_ptr<QString> m_alias = std::make_shared<QString>();
    std::shared_ptr<double> m_urValue = std::make_shared<double>(0.0);
    std::shared_ptr<double> m_rejection = std::make_shared<double>(0.0);
    std::shared_ptr<double> m_ovrejection = std::make_shared<double>(0.0);
    std::shared_ptr<double> m_adcrejection = std::make_shared<double>(0.0);

    TaskContainerInterfacePtr m_currentTasks;
};

typedef std::shared_ptr<Range> RangePtr;

}

#endif // CRO_RANGE_H

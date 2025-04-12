#include "cro_systemobserver.h"
#include "cro_channelfetchtask.h"
#include "taskchannelgetavail.h"
#include "taskgetsamplerate.h"
#include "taskserverconnectionstart.h"
#include <taskcontainersequence.h>
#include <tasklambdarunner.h>
#include <taskcontainerparallel.h>
#include <proxy.h>

namespace ChannelRangeObserver {

SystemObserver::SystemObserver(const NetworkConnectionInfo &netInfo, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory))
{
}

void SystemObserver::startFullScan()
{
    if(m_channelMNameToChannel.isEmpty())
        doStartFullScan();
    else
        emit sigFullScanFinished(true);
}

const QStringList SystemObserver::getChannelMNames() const
{
    return m_channelMNameToChannel.keys();
}

const QStringList SystemObserver::getChannelAliases() const
{
    QStringList aliases;
    for(auto iter=m_channelMNameToChannel.constBegin(); iter!=m_channelMNameToChannel.constEnd(); iter++)
        aliases.append(iter.value()->getAlias());
    return aliases;
}

const ChannelPtr SystemObserver::getChannel(QString channelMName) const
{
    auto iter = m_channelMNameToChannel.constFind(channelMName);
    if(iter != m_channelMNameToChannel.constEnd())
        return iter.value();
    qWarning("SystemObserver: Channel data not found for %s!", qPrintable(channelMName));
    return std::make_shared<Channel>("", NetworkConnectionInfo(), m_tcpFactory);
}

const int SystemObserver::getSamplesPerPeriod() const
{
    return *m_samplesPerPeriod;
}

void SystemObserver::clear()
{
    m_channelMNameToChannel.clear();
}

void SystemObserver::preparePcbInterface()
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
}

void SystemObserver::doStartFullScan()
{
    preparePcbInterface();
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());
    m_tempChannelMNames = std::make_shared<QStringList>();
    m_currentTasks->addSub(TaskChannelGetAvail::create(m_pcbInterface, m_tempChannelMNames,
                                                       TRANSACTION_TIMEOUT, [=] { notifyError("Get available channels failed");}));
    m_currentTasks->addSub(TaskLambdaRunner::create([&]() {
        TaskContainerInterfacePtr allChannelsDetailsTasks = TaskContainerParallel::create();
        allChannelsDetailsTasks->addSub(TaskGetSampleRate::create(m_pcbInterface, m_samplesPerPeriod,
                                                                  TRANSACTION_TIMEOUT, [=] { notifyError("Get sample rate failed");}));
        for(const QString &channelMName : *m_tempChannelMNames) {
            ChannelPtr channelObserver = std::make_shared<Channel>(channelMName, m_netInfo, m_tcpFactory);
            m_channelMNameToChannel[channelMName] = channelObserver;

            allChannelsDetailsTasks->addSub(ChannelFetchTask::create(channelObserver));
            connect(channelObserver.get(), &Channel::sigFetchDoneChannel,
                    this, &SystemObserver::sigFetchDone);
        }
        m_currentTasks->addSub(std::move(allChannelsDetailsTasks));
        return true;
    }));

    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &SystemObserver::sigFullScanFinished);
    m_currentTasks->start();
}

TaskTemplatePtr SystemObserver::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

void SystemObserver::notifyError(QString errMsg)
{
    qWarning("SystemObserver error: %s", qPrintable(errMsg));
}

}

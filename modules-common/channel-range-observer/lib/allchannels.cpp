#include "allchannels.h"
#include "channelfetchtask.h"
#include "taskchannelgetavail.h"
#include "taskserverconnectionstart.h"
#include <taskcontainersequence.h>
#include <tasklambdarunner.h>
#include <taskcontainerparallel.h>
#include <proxy.h>

namespace ChannelRangeObserver {

AllChannels::AllChannels(const NetworkConnectionInfo &netInfo, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory)),
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{
    m_pcbInterface->setClientSmart(m_pcbClient);
}

void AllChannels::startFullScan()
{
    if(m_channelNameToChannel.isEmpty())
        doStartFullScan();
    else
        emit sigFullScanFinished(true);
}

const QStringList AllChannels::getChannelMNames() const
{
    return m_channelNameToChannel.keys();
}

const ChannelPtr AllChannels::getChannel(QString channelMName)
{
    auto iter = m_channelNameToChannel.constFind(channelMName);
    if(iter != m_channelNameToChannel.constEnd())
        return iter.value();
    qWarning("AllChannels: Channel data not found for %s!", qPrintable(channelMName));
    return nullptr;
}

void AllChannels::clear()
{
    m_channelNameToChannel.clear();
}

void AllChannels::doStartFullScan()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());
    m_currentTasks->addSub(TaskChannelGetAvail::create(m_pcbInterface, m_tempChannelMNames,
                                                       TRANSACTION_TIMEOUT, [=] { notifyError("Get available channels failed");}));
    m_currentTasks->addSub(TaskLambdaRunner::create([&]() {
        TaskContainerInterfacePtr allChannelsDetailsTasks = TaskContainerParallel::create();
        for(const QString &channelMName : qAsConst(m_tempChannelMNames)) {
            ChannelPtr channelObserver = std::make_shared<Channel>(channelMName, m_netInfo, m_tcpFactory);
            m_channelNameToChannel[channelMName] = channelObserver;

            allChannelsDetailsTasks->addSub(ChannelFetchTask::create(channelObserver));
            connect(channelObserver.get(), &Channel::sigFetchComplete,
                    this, &AllChannels::sigFetchComplete);
        }
        m_tempChannelMNames.clear();
        m_currentTasks->addSub(std::move(allChannelsDetailsTasks));
        return true;
    }));

    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &AllChannels::sigFullScanFinished);
    m_currentTasks->start();
}

TaskTemplatePtr AllChannels::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

void AllChannels::notifyError(QString errMsg)
{
    qWarning("AllChannels error: %s", qPrintable(errMsg));
}

}

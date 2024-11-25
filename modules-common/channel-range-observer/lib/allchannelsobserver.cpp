#include "allchannelsobserver.h"
#include "channelfetchtask.h"
#include "taskchannelgetavail.h"
#include "taskserverconnectionstart.h"
#include <taskcontainersequence.h>
#include <tasklambdarunner.h>
#include <taskcontainerparallel.h>
#include <proxy.h>

namespace ChannelRangeObserver {

AllChannelsObserver::AllChannelsObserver(const NetworkConnectionInfo &netInfo, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory)),
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{
    m_pcbInterface->setClientSmart(m_pcbClient);
}

void AllChannelsObserver::startFullScan()
{
    if(m_channelNamesToObserver.isEmpty())
        doStartFullScan();
    else
        emit sigFullScanFinished(true);
}

const QStringList AllChannelsObserver::getChannelMNames() const
{
    return m_channelNamesToObserver.keys();
}

const ChannelObserverPtr AllChannelsObserver::getChannelObserver(QString channelMName)
{
    auto iter = m_channelNamesToObserver.constFind(channelMName);
    if(iter != m_channelNamesToObserver.constEnd())
        return iter.value();
    qWarning("AllChannelsObserver: Channel data not found for %s!", qPrintable(channelMName));
    return nullptr;
}

void AllChannelsObserver::clear()
{
    m_channelNamesToObserver.clear();
}

void AllChannelsObserver::doStartFullScan()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());
    m_currentTasks->addSub(TaskChannelGetAvail::create(m_pcbInterface, m_tempChannelMNames,
                                                       TRANSACTION_TIMEOUT, [=] { notifyError("Get available channels failed");}));
    m_currentTasks->addSub(TaskLambdaRunner::create([&]() {
        TaskContainerInterfacePtr allChannelsDetailsTasks = TaskContainerParallel::create();
        for(const QString &channelMName : qAsConst(m_tempChannelMNames)) {
            ChannelObserverPtr channelObserver = std::make_shared<ChannelObserver>(channelMName, m_netInfo, m_tcpFactory);
            m_channelNamesToObserver[channelMName] = channelObserver;

            allChannelsDetailsTasks->addSub(ChannelFetchTask::create(channelObserver));
            connect(channelObserver.get(), &ChannelObserver::sigFetchComplete,
                    this, &AllChannelsObserver::sigFetchComplete);
        }
        m_tempChannelMNames.clear();
        m_currentTasks->addSub(std::move(allChannelsDetailsTasks));
        return true;
    }));

    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &AllChannelsObserver::sigFullScanFinished);
    m_currentTasks->start();
}

TaskTemplatePtr AllChannelsObserver::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

void AllChannelsObserver::notifyError(QString errMsg)
{
    qWarning("AllChannelsObserver error: %s", qPrintable(errMsg));
}

}

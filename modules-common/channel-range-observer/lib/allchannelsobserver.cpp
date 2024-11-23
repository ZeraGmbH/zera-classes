#include "allchannelsobserver.h"
#include "taskchannelgetavail.h"
#include "taskserverconnectionstart.h"
#include <taskcontainersequence.h>
#include <taskchannelgetalias.h>
#include <taskchannelgetdspchannel.h>
#include <taskchannelgetunit.h>
#include <tasklambdarunner.h>
#include <taskcontainerparallel.h>
#include <proxy.h>

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
    m_notifyIdToChannelMName.clear();
}

void AllChannelsObserver::startRangesChangeTasks(QString channelMName)
{
    auto iter = m_channelNamesToObserver.find(channelMName);
    if(iter != m_channelNamesToObserver.end())
        iter.value()->startFetch();
    else
        qInfo("AllChannelsObserver: On change notification channel %s not found!", qPrintable(channelMName));
}

void AllChannelsObserver::doStartFullScan()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());

    m_currentTasks->addSub(TaskChannelGetAvail::create(
        m_pcbInterface, m_tempChannelMNames,
        TRANSACTION_TIMEOUT, [=] { notifyError("Get available channels failed");}));

    m_currentTasks->addSub(TaskLambdaRunner::create([&]() {
        TaskContainerInterfacePtr allChannelsDetailsTasks = TaskContainerParallel::create();
        for(int channelNo = 0; channelNo<m_tempChannelMNames.count(); ++channelNo) {
            QString &channelMName = m_tempChannelMNames[channelNo];
            ChannelObserverPtr channelObserver = std::make_shared<ChannelObserver>(channelMName, m_netInfo, m_tcpFactory);
            m_channelNamesToObserver[channelMName] = channelObserver;

            // TODO this has to go
            TaskContainerInterfacePtr perChannelTask = TaskContainerSequence::create();
            perChannelTask->addSub(getChannelReadDetailsTasks(channelMName, channelNo));
            perChannelTask = channelObserver->addRangesFetchTasks(std::move(perChannelTask));
            allChannelsDetailsTasks->addSub(std::move(perChannelTask));

            connect(channelObserver.get(), &ChannelObserver::sigFetchComplete,
                    this, &AllChannelsObserver::sigFetchComplete);
        }
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

TaskTemplatePtr AllChannelsObserver::getChannelReadDetailsTasks(const QString &channelMName, int notificationId)
{
    TaskContainerInterfacePtr channelTasks = TaskContainerParallel::create();
    m_notifyIdToChannelMName[notificationId] = channelMName;
    channelTasks->addSub(TaskChannelGetAlias::create(
        m_pcbInterface, channelMName, m_channelNamesToObserver[channelMName]->m_alias,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read alias for channel %1").arg(channelMName)); }));
    channelTasks->addSub(TaskChannelGetDspChannel::create(
        m_pcbInterface, channelMName, m_channelNamesToObserver[channelMName]->m_dspChannel,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read dsp-channel for channel %1").arg(channelMName)); }));
    channelTasks->addSub(TaskChannelGetUnit::create(
        m_pcbInterface, channelMName, m_channelNamesToObserver[channelMName]->m_unit,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read unit for channel %1").arg(channelMName)); }));
    return channelTasks;
}

void AllChannelsObserver::notifyError(QString errMsg)
{
    qWarning("AllChannelsObserver error: %s", qPrintable(errMsg));
}


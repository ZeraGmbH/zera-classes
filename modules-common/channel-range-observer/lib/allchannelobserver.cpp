#include "allchannelobserver.h"
#include "taskchannelgetavail.h"
#include "taskserverconnectionstart.h"
#include <taskcontainersequence.h>
#include <taskchannelgetalias.h>
#include <taskchannelgetdspchannel.h>
#include <taskchannelgetunit.h>
#include <tasklambdarunner.h>
#include <taskcontainerparallel.h>
#include <proxy.h>

AllChannelObserver::AllChannelObserver(const NetworkConnectionInfo &netInfo, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory)),
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{
    m_pcbInterface->setClientSmart(m_pcbClient);
}

void AllChannelObserver::startFullScan()
{
    if(m_channelNamesToObserver.isEmpty())
        doStartFullScan();
    else
        emit sigFullScanFinished(true);
}

const QStringList AllChannelObserver::getChannelMNames() const
{
    return m_channelNamesToObserver.keys();
}

const QStringList AllChannelObserver::getChannelRanges(const QString &channelMName) const
{
    auto iterChannel = m_channelNamesToObserver.constFind(channelMName);
    if(iterChannel != m_channelNamesToObserver.constEnd())
        return iterChannel.value()->getRangeNames();
    qWarning("AllChannelObserver: Channel ranges not found for %s!", qPrintable(channelMName));
    return QStringList();
}

const RangeObserverPtr AllChannelObserver::getChannelData(QString channelMName)
{
    auto iter = m_channelNamesToObserver.constFind(channelMName);
    if(iter != m_channelNamesToObserver.constEnd())
        return iter.value();
    qWarning("AllChannelObserver: Channel data not found for %s!", qPrintable(channelMName));
    return nullptr;
}

void AllChannelObserver::clear()
{
    m_channelNamesToObserver.clear();
    m_notifyIdToChannelMName.clear();
}

void AllChannelObserver::startRangesChangeTasks(QString channelMName)
{
    auto iter = m_channelNamesToObserver.find(channelMName);
    if(iter != m_channelNamesToObserver.end())
        iter.value()->startReadRanges();
    else
        qInfo("AllChannelObserver: On change notification channel %s not found!", qPrintable(channelMName));
}

void AllChannelObserver::doStartFullScan()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(perparePcbConnectionTasks());

    m_currentTasks->addSub(TaskChannelGetAvail::create(
        m_pcbInterface, m_tempChannelMNames,
        TRANSACTION_TIMEOUT, [=] { notifyError("Get available channels failed");}));

    m_currentTasks->addSub(TaskLambdaRunner::create([&]() {
        TaskContainerInterfacePtr allChannelsDetailsTasks = TaskContainerParallel::create();
        for(int channelNo = 0; channelNo<m_tempChannelMNames.count(); ++channelNo) {
            QString &channelMName = m_tempChannelMNames[channelNo];
            RangeObserverPtr channelObserver = std::make_shared<RangeObserver>(channelMName, m_netInfo, m_tcpFactory);
            m_channelNamesToObserver[channelMName] = channelObserver;

            // TODO this has to go
            TaskContainerInterfacePtr perChannelTask = TaskContainerSequence::create();
            perChannelTask->addSub(getChannelReadDetailsTasks(channelMName, channelNo));
            perChannelTask = channelObserver->addRangesFetchTasks(std::move(perChannelTask));
            allChannelsDetailsTasks->addSub(std::move(perChannelTask));

            connect(channelObserver.get(), &RangeObserver::sigRangeListChanged,
                    this, &AllChannelObserver::sigRangeListChanged);
        }
        m_currentTasks->addSub(std::move(allChannelsDetailsTasks));
        return true;
    }));

    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &AllChannelObserver::sigFullScanFinished);
    m_currentTasks->start();
}

TaskTemplatePtr AllChannelObserver::perparePcbConnectionTasks()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

TaskTemplatePtr AllChannelObserver::getChannelReadDetailsTasks(const QString &channelMName, int notificationId)
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

void AllChannelObserver::notifyError(QString errMsg)
{
    qWarning("AllChannelObserver error: %s", qPrintable(errMsg));
}


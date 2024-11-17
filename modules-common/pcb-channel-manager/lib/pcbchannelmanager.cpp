#include "pcbchannelmanager.h"
#include "taskchannelgetavail.h"
#include "taskserverconnectionstart.h"
#include <taskcontainersequence.h>
#include <taskchannelgetalias.h>
#include <taskchannelgetdspchannel.h>
#include <taskchannelgetunit.h>
#include <taskchannelgetrangelist.h>
#include <tasklambdarunner.h>
#include <taskcontainerparallel.h>

void PcbChannelManager::startScan(Zera::ProxyClientPtr pcbClient)
{
    if(m_channels.isEmpty()) {
        createTasks(pcbClient);
        connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
                this, &PcbChannelManager::onTasksFinish);
        m_currentTasks->start();
    }
    else
        emit sigScanFinished(true);
}

const QStringList PcbChannelManager::getChannelMNames() const
{
    return m_channels.keys();
}

const QStringList PcbChannelManager::getChannelRanges(const QString &channelMName) const
{
    QStringList ranges;
    auto iter = m_tempChannelRanges.constFind(channelMName);
    if(iter != m_tempChannelRanges.constEnd())
        ranges = iter.value();
    return ranges;
}

const PcbChannelDataPtr PcbChannelManager::getChannelData(QString channelName)
{
    auto iter = m_channels.constFind(channelName);
    if(iter != m_channels.constEnd())
        return iter.value();
    qWarning("PcbChannelManager: Channel data for %s not found!", qPrintable(channelName));
    return nullptr;
}

void PcbChannelManager::onTasksFinish(bool ok)
{
    m_currentTasks = nullptr;
    emit sigScanFinished(ok);
}

void PcbChannelManager::createTasks(Zera::ProxyClientPtr pcbClient)
{
    m_currentTasks = TaskContainerSequence::create();

    m_currentTasks->addSub(TaskServerConnectionStart::create(pcbClient, CONNECTION_TIMEOUT));

    Zera::PcbInterfacePtr pcbInterface = std::make_shared<Zera::cPCBInterface>();
    pcbInterface->setClientSmart(pcbClient);
    m_currentTasks->addSub(TaskChannelGetAvail::create(
        pcbInterface, m_tempChannelMNames, TRANSACTION_TIMEOUT, [=] { notifyError("Get available channels failed");}));

    m_currentTasks->addSub(TaskLambdaRunner::create([=]() {
        m_currentTasks->addSub(getChannelsReadTasks(pcbInterface));
        return true;
    }));
}

void PcbChannelManager::notifyError(QString errMsg)
{
    qWarning("PcbChannelManager error: %s", qPrintable(errMsg));
}

TaskTemplatePtr PcbChannelManager::getChannelsReadTasks(Zera::PcbInterfacePtr pcbInterface)
{
    TaskContainerInterfacePtr channelTasks = TaskContainerParallel::create();
    for(const auto &channelName : qAsConst(m_tempChannelMNames)) {
        m_channels[channelName] = std::make_shared<PcbChannelData>();
        channelTasks->addSub(TaskChannelGetAlias::create(pcbInterface,
                                                         channelName,
                                                         m_channels[channelName]->m_alias,
                                                         TRANSACTION_TIMEOUT,
                                                         [&]{ notifyError(QString("Could not read alias for channel %1").arg(channelName)); }));
        channelTasks->addSub(TaskChannelGetDspChannel::create(pcbInterface,
                                                              channelName,
                                                              m_channels[channelName]->m_dspChannel,
                                                              TRANSACTION_TIMEOUT,
                                                              [&]{ notifyError(QString("Could not read dsp-channel for channel %1").arg(channelName)); }));
        channelTasks->addSub(TaskChannelGetUnit::create(pcbInterface,
                                                        channelName,
                                                        m_channels[channelName]->m_unit,
                                                        TRANSACTION_TIMEOUT,
                                                        [&]{ notifyError(QString("Could not read unit for channel %1").arg(channelName)); }));
        channelTasks->addSub(TaskChannelGetRangeList::create(pcbInterface,
                                                             channelName,
                                                             m_tempChannelRanges[channelName],
                                                             TRANSACTION_TIMEOUT,
                                                             [&]{ notifyError(QString("Could not read range list for channel %1").arg(channelName)); }));
    }
    return channelTasks;
}

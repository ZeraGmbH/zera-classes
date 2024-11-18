#include "pcbchannelmanager.h"
#include "taskchannelgetavail.h"
#include "taskserverconnectionstart.h"
#include <taskcontainersequence.h>
#include <taskchannelgetalias.h>
#include <taskchannelgetdspchannel.h>
#include <taskchannelgetunit.h>
#include <taskchannelgetrangelist.h>
#include <taskchannelregisternotifier.h>
#include <tasklambdarunner.h>
#include <taskcontainerparallel.h>

void PcbChannelManager::startScan(Zera::ProxyClientPtr pcbClient)
{
    if(m_channelNamesToData.isEmpty()) {
        prepareScan(pcbClient);
        m_currentTasks->start();
    }
    else
        emit sigScanFinished(true);
}

const QStringList PcbChannelManager::getChannelMNames() const
{
    return m_channelNamesToData.keys();
}

const QStringList PcbChannelManager::getChannelRanges(const QString &channelMName) const
{
    QStringList ranges;
    auto iterChannel = m_channelNamesToData.constFind(channelMName);
    if(iterChannel != m_channelNamesToData.constEnd())
        ranges = iterChannel.value()->m_tempChannelRanges;
    return ranges;
}

const PcbChannelDataPtr PcbChannelManager::getChannelData(QString channelMName)
{
    auto iter = m_channelNamesToData.constFind(channelMName);
    if(iter != m_channelNamesToData.constEnd())
        return iter.value();
    qWarning("PcbChannelManager: Channel data for %s not found!", qPrintable(channelMName));
    return nullptr;
}

Zera::PcbInterfacePtr PcbChannelManager::getPcbInterface()
{
    return m_currentPcbInterface;
}

void PcbChannelManager::onTasksFinish(bool ok)
{
    m_currentTasks = nullptr;
    emit sigScanFinished(ok);
}

void PcbChannelManager::onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        const QStringList answerParts = answer.toString().split(":", Qt::SkipEmptyParts);
        if(answerParts.size() == 2 && answerParts[0] == "Notify") {
            bool ok;
            auto iter = m_notifyIdToChannelMName.constFind(answerParts[1].toInt(&ok));
            if(ok && iter != m_notifyIdToChannelMName.constEnd()) {
                QString channelMName = iter.value();
                startRangesChangeTasks(channelMName);
            }
            else
                qInfo("PcbChannelManager: Unknown notification: %s!", qPrintable(answer.toString()));
        }
    }
}

void PcbChannelManager::startRangesChangeTasks(QString channelMName)
{
    auto iter = m_channelNamesToData.find(channelMName);
    if(iter != m_channelNamesToData.end()) {
        TaskContainerInterfacePtr rangeTasks = TaskContainerSequence::create();
        const QStringList channelMNames = QStringList() << channelMName;
        rangeTasks->addSub(getChannelReadRangeNamesTask(channelMName));
        rangeTasks->addSub(getReadRangeDetailsTasks(channelMNames));
        rangeTasks->addSub(getReadRangeFinalTasks(channelMNames));
        iter.value()->m_currentTasks = std::move(rangeTasks);
        iter.value()->m_currentTasks->start();
    }
    else
        qInfo("PcbChannelManager: On change notification channel %s not found!", qPrintable(channelMName));
}

void PcbChannelManager::prepareScan(Zera::ProxyClientPtr pcbClient)
{
    m_currentPcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_currentPcbInterface->setClientSmart(pcbClient);
    connect(m_currentPcbInterface.get(), &Zera::cPCBInterface::serverAnswer,
            this, &PcbChannelManager::onInterfaceAnswer);

    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(TaskServerConnectionStart::create(pcbClient, CONNECTION_TIMEOUT));
    m_currentTasks->addSub(TaskChannelGetAvail::create(
        m_currentPcbInterface, m_tempChannelMNames,
        TRANSACTION_TIMEOUT, [=] { notifyError("Get available channels failed");}));
    m_currentTasks->addSub(TaskLambdaRunner::create([=]() {
        m_currentTasks->addSub(getChannelsReadInfoAndRangeNamesTasks(m_tempChannelMNames));
        m_tempChannelMNames.clear(); // avoid further usage
        return true;
    }));

    m_currentTasks->addSub(TaskLambdaRunner::create([=]() {
        m_currentTasks->addSub(getReadRangeDetailsTasks(getChannelMNames()));
        m_currentTasks->addSub(getReadRangeFinalTasks(getChannelMNames()));
        return true;
    }));

    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &PcbChannelManager::onTasksFinish);
}

void PcbChannelManager::notifyError(QString errMsg)
{
    qWarning("PcbChannelManager error: %s", qPrintable(errMsg));
}

TaskTemplatePtr PcbChannelManager::getChannelsReadInfoAndRangeNamesTasks(const QStringList &channelMNames)
{
    TaskContainerInterfacePtr allChannelsTasks = TaskContainerParallel::create();
    for(int i=0; i<channelMNames.size(); ++i) {
        TaskContainerInterfacePtr channelTasks = TaskContainerSequence::create();
        const QString &channelMName = channelMNames[i];
        m_notifyIdToChannelMName[i] = channelMName;
        m_channelNamesToData[channelMName] = std::make_shared<PcbChannelData>();
        channelTasks->addSub(TaskChannelGetAlias::create(
            m_currentPcbInterface, channelMName, m_channelNamesToData[channelMName]->m_alias,
            TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read alias for channel %1").arg(channelMName)); }));
        channelTasks->addSub(TaskChannelGetDspChannel::create(
            m_currentPcbInterface, channelMName, m_channelNamesToData[channelMName]->m_dspChannel,
            TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read dsp-channel for channel %1").arg(channelMName)); }));
        channelTasks->addSub(TaskChannelGetUnit::create(
            m_currentPcbInterface, channelMName, m_channelNamesToData[channelMName]->m_unit,
            TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read unit for channel %1").arg(channelMName)); }));
        // TODO: Handle / check unregister
        channelTasks->addSub(TaskChannelRegisterNotifier::create(
            m_currentPcbInterface, channelMName, i,
            TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register notification for channel %1").arg(channelMName)); }));
        channelTasks->addSub(getChannelReadRangeNamesTask(channelMName));
        allChannelsTasks->addSub(std::move(channelTasks));
    }
    return allChannelsTasks;
}

TaskTemplatePtr PcbChannelManager::getChannelReadRangeNamesTask(const QString &channelMName)
{
    return TaskChannelGetRangeList::create(
        m_currentPcbInterface, channelMName, m_channelNamesToData[channelMName]->m_tempChannelRanges,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read range list for channel %1").arg(channelMName)); });
}

TaskTemplatePtr PcbChannelManager::getReadRangeDetailsTasks(const QStringList &channelMNames)
{
    TaskContainerInterfacePtr allChannelsTasks = TaskContainerParallel::create();
    for(const QString &channelMName : channelMNames) {
        // TODO: add range tasks
    }
    return allChannelsTasks;
}

TaskTemplatePtr PcbChannelManager::getReadRangeFinalTasks(const QStringList &channelMNames)
{
    TaskContainerInterfacePtr allChannelsTasks = TaskContainerParallel::create();
    for(const QString &channelMName : channelMNames)
        emit sigChannelRangesChanged(channelMName);
    return allChannelsTasks;
}

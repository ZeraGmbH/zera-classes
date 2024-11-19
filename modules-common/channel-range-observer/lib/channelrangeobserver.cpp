#include "channelrangeobserver.h"
#include "taskchannelgetavail.h"
#include "taskserverconnectionstart.h"
#include <taskcontainersequence.h>
#include <taskchannelgetalias.h>
#include <taskchannelgetdspchannel.h>
#include <taskchannelgetunit.h>
#include <taskchannelregisternotifier.h>
#include <tasklambdarunner.h>
#include <taskcontainerparallel.h>

void ChannelRangeObserver::startFullScan(Zera::ProxyClientPtr pcbClient)
{
    if(m_channelNamesToObserver.isEmpty())
        doStartFullScan(pcbClient);
    else
        emit sigFullScanFinished(true);
}

const QStringList ChannelRangeObserver::getChannelMNames() const
{
    return m_channelNamesToObserver.keys();
}

const QStringList ChannelRangeObserver::getChannelRanges(const QString &channelMName) const
{
    auto iterChannel = m_channelNamesToObserver.constFind(channelMName);
    if(iterChannel != m_channelNamesToObserver.constEnd())
        return iterChannel.value()->getRangeNames();
    qWarning("ChannelRangeObserver: Channel ranges not found for %s!", qPrintable(channelMName));
    return QStringList();
}

const ChannelObserverPtr ChannelRangeObserver::getChannelData(QString channelMName)
{
    auto iter = m_channelNamesToObserver.constFind(channelMName);
    if(iter != m_channelNamesToObserver.constEnd())
        return iter.value();
    qWarning("ChannelRangeObserver: Channel data not found for %s!", qPrintable(channelMName));
    return nullptr;
}

void ChannelRangeObserver::onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        const QStringList answerParts = answer.toString().split(":", Qt::SkipEmptyParts);
        if(answerParts.size() == 2 && answerParts[0] == "Notify") {
            bool ok;
            auto iter = m_notifyIdToChannelMName.constFind(answerParts[1].toInt(&ok));
            if(ok && iter != m_notifyIdToChannelMName.constEnd())
                startRangesChangeTasks(iter.value());
            else
                qInfo("ChannelRangeObserver: Unknown notification: %s!", qPrintable(answer.toString()));
        }
        else
            qInfo("ChannelRangeObserver: Unknown notification: %s!", qPrintable(answer.toString()));
    }
}

void ChannelRangeObserver::startRangesChangeTasks(QString channelMName)
{
    auto iter = m_channelNamesToObserver.find(channelMName);
    if(iter != m_channelNamesToObserver.end())
        iter.value()->startReReadRanges(channelMName, m_currentPcbInterface);
    else
        qInfo("ChannelRangeObserver: On change notification channel %s not found!", qPrintable(channelMName));
}

void ChannelRangeObserver::doStartFullScan(Zera::ProxyClientPtr pcbClient)
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(perparePcbConnectionTasks(pcbClient));

    m_currentTasks->addSub(TaskChannelGetAvail::create(
        m_currentPcbInterface, m_tempChannelMNames,
        TRANSACTION_TIMEOUT, [=] { notifyError("Get available channels failed");}));

    m_currentTasks->addSub(TaskLambdaRunner::create([&]() {
        TaskContainerInterfacePtr allChannelsDetailsTasks = TaskContainerParallel::create();
        for(int channelNo = 0; channelNo<m_tempChannelMNames.count(); ++channelNo) {
            ChannelObserverPtr channelObserver = std::make_shared<ChannelObserver>();
            QString &channelMName = m_tempChannelMNames[channelNo];
            m_channelNamesToObserver[channelMName] = channelObserver;

            TaskContainerInterfacePtr perChannelTask = TaskContainerSequence::create();
            perChannelTask->addSub(getChannelReadDetailsTasks(channelMName, channelNo));
            perChannelTask->addSub(channelObserver->getRangesFetchTasks(channelMName, m_currentPcbInterface));
            allChannelsDetailsTasks->addSub(std::move(perChannelTask));

            connect(channelObserver.get(), &ChannelObserver::sigRangeListChanged,
                    this, &ChannelRangeObserver::sigRangeListChanged);
        }
        m_currentTasks->addSub(std::move(allChannelsDetailsTasks));
        return true;
    }));

    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &ChannelRangeObserver::sigFullScanFinished);
    m_currentTasks->start();
}

TaskTemplatePtr ChannelRangeObserver::perparePcbConnectionTasks(Zera::ProxyClientPtr pcbClient)
{
    m_currentPcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_currentPcbInterface->setClientSmart(pcbClient);
    connect(m_currentPcbInterface.get(), &Zera::cPCBInterface::serverAnswer,
            this, &ChannelRangeObserver::onInterfaceAnswer);
    return TaskServerConnectionStart::create(pcbClient, CONNECTION_TIMEOUT);
}

TaskTemplatePtr ChannelRangeObserver::getChannelReadDetailsTasks(const QString &channelMName, int notificationId)
{
    TaskContainerInterfacePtr channelTasks = TaskContainerParallel::create();
    m_notifyIdToChannelMName[notificationId] = channelMName;
    channelTasks->addSub(TaskChannelGetAlias::create(
        m_currentPcbInterface, channelMName, m_channelNamesToObserver[channelMName]->m_alias,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read alias for channel %1").arg(channelMName)); }));
    channelTasks->addSub(TaskChannelGetDspChannel::create(
        m_currentPcbInterface, channelMName, m_channelNamesToObserver[channelMName]->m_dspChannel,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read dsp-channel for channel %1").arg(channelMName)); }));
    channelTasks->addSub(TaskChannelGetUnit::create(
        m_currentPcbInterface, channelMName, m_channelNamesToObserver[channelMName]->m_unit,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read unit for channel %1").arg(channelMName)); }));
    // TODO: Handle / check unregister
    channelTasks->addSub(TaskChannelRegisterNotifier::create(
        m_currentPcbInterface, channelMName, notificationId,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register notification for channel %1").arg(channelMName)); }));
    return channelTasks;
}

void ChannelRangeObserver::notifyError(QString errMsg)
{
    qWarning("ChannelRangeObserver error: %s", qPrintable(errMsg));
}


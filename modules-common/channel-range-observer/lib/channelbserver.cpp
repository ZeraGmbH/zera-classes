#include "channelbserver.h"
#include "taskserverconnectionstart.h"
#include <taskchannelgetrangelist.h>
#include <taskcontainersequence.h>
#include <taskcontainerparallel.h>
#include <taskchannelgetalias.h>
#include <taskchannelgetdspchannel.h>
#include <taskchannelgetunit.h>
#include <tasklambdarunner.h>
#include <taskchannelregisternotifier.h>
#include <proxy.h>

ChannelObserver::ChannelObserver(const QString &channelMName,
                                 const NetworkConnectionInfo &netInfo,
                                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_channelMName(channelMName),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory)),
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{
    m_pcbInterface->setClientSmart(m_pcbClient);
}

const QStringList ChannelObserver::getRangeNames() const
{
    return m_tempRangesNames;
}

void ChannelObserver::startFetch()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());
    m_currentTasks->addSub(getChannelReadDetailsTask());
    m_currentTasks->addSub(getRangesFetchTasks());
    m_currentTasks->start();
}

void ChannelObserver::onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        const QStringList answerParts = answer.toString().split(":", Qt::SkipEmptyParts);
        if(answerParts.size() == 2 && answerParts[0] == "Notify")
            startFetch();
        else
            qInfo("onInterfaceAnswer: Unknown notification: %s!", qPrintable(answer.toString()));
    }
}

TaskTemplatePtr ChannelObserver::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

TaskTemplatePtr ChannelObserver::getChannelReadDetailsTask()
{
    TaskContainerInterfacePtr task = TaskContainerParallel::create();
    task->addSub(TaskChannelGetAlias::create(
        m_pcbInterface, m_channelMName, m_alias,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read alias for channel %1").arg(m_channelMName)); }));
    task->addSub(TaskChannelGetDspChannel::create(
        m_pcbInterface, m_channelMName, m_dspChannel,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read dsp-channel for channel %1").arg(m_channelMName)); }));
    task->addSub(TaskChannelGetUnit::create(
        m_pcbInterface, m_channelMName, m_unit,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read unit for channel %1").arg(m_channelMName)); }));
    return task;
}

TaskTemplatePtr ChannelObserver::getReadRangeNamesTask()
{
    return TaskChannelGetRangeList::create(
        m_pcbInterface, m_channelMName, m_tempRangesNames,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read range list for channel %1").arg(m_channelMName)); });
}

TaskTemplatePtr ChannelObserver::getRangesFetchTasks()
{
    TaskContainerInterfacePtr task = TaskContainerSequence::create();
    task->addSub(getReadRangeNamesTask());
    // TODO: Handle / check unregister
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer,
            this, &ChannelObserver::onInterfaceAnswer);
    task->addSub(TaskChannelRegisterNotifier::create(
        m_pcbInterface, m_channelMName, 1,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register notification for channel %1").arg(m_channelMName)); }));

    task->addSub(getReadRangeDetailsTask());
    task->addSub(getReadRangeFinalTask());
    return task;
}

TaskTemplatePtr ChannelObserver::getReadRangeDetailsTask()
{
    TaskContainerInterfacePtr taskToAdd = TaskContainerParallel::create();

    // TODO: add range tasks

    return taskToAdd;
}

TaskTemplatePtr ChannelObserver::getReadRangeFinalTask()
{
    return TaskLambdaRunner::create([=]() {
        emit sigFetchComplete(m_channelMName);
        return true;
    });
}

void ChannelObserver::notifyError(QString errMsg)
{
    qWarning("ChannelObserver error: %s", qPrintable(errMsg));
}

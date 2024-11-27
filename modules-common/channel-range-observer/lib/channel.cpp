#include "channel.h"
#include "taskrangegeturvalue.h"
#include "taskrangegetisavailable.h"
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

namespace ChannelRangeObserver {

Channel::Channel(const QString &channelMName,
                 const NetworkConnectionInfo &netInfo,
                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_channelMName(channelMName),
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory)),
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{
    m_pcbInterface->setClientSmart(m_pcbClient);
}

void Channel::startFetch()
{
    clearRanges();
    startAllRangesTasks();
}

const QStringList Channel::getAllRangeNames() const
{
    return m_allRangeNamesOrderedByServer;
}

const QStringList Channel::getAvailRangeNames() const
{
    return m_availableRangeNames;
}

const std::shared_ptr<Range> Channel::getRange(const QString &rangeName) const
{
    auto iter = m_rangeNameToRange.constFind(rangeName);
    if(iter != m_rangeNameToRange.constEnd())
        return iter.value();
    qWarning("Range data for range %s not found!", qPrintable(rangeName));
    return nullptr;
}

void Channel::onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(reply)
    // TODO?: Remove all decoding: We expect nothing but our notification - but howto test?
    if (msgnr == 0) { // 0 was reserved for async. messages
        const QStringList answerParts = answer.toString().split(":", Qt::SkipEmptyParts);
        if(answerParts.size() == 2 && answerParts[0] == "Notify")
            startFetch();
        else
            qInfo("onInterfaceAnswer: Unknown notification: %s!", qPrintable(answer.toString()));
    }
}

void Channel::clearRanges()
{
    m_allRangeNamesOrderedByServer.clear();
    m_availableRangeNames.clear();
    m_rangeNameToRange.clear();
}

TaskTemplatePtr Channel::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

void Channel::startAllRangesTasks()
{
    m_currentTasks = TaskContainerSequence::create();
    TaskContainerInterfacePtr task = TaskContainerSequence::create();
    task->addSub(getPcbConnectionTask());
    task->addSub(TaskChannelGetRangeList::create(
        m_pcbInterface, m_channelMName, m_allRangeNamesOrderedByServer,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read range list for channel %1").arg(m_channelMName)); }));
    task->addSub(TaskLambdaRunner::create([&]() {
        TaskContainerInterfacePtr allRangesTasks = TaskContainerParallel::create();
        allRangesTasks->addSub(getChannelReadDetailsTask());
        for(const QString &rangeName : qAsConst(m_allRangeNamesOrderedByServer)) {
            std::shared_ptr<Range> newRange = std::make_shared<Range>(m_channelMName, rangeName, m_netInfo, m_tcpFactory);
            m_rangeNameToRange[rangeName] = newRange;
            allRangesTasks = addRangeDataTasks(std::move(allRangesTasks), rangeName, newRange);
        }
        m_currentTasks->addSub(std::move(allRangesTasks));
        m_currentTasks->addSub(getRangesRegisterChangeNotificationTask());
        m_currentTasks->addSub(getFetchFinalTask());
        return true;
    }));
    m_currentTasks->addSub(std::move(task));
    m_currentTasks->start();
}

TaskTemplatePtr Channel::getChannelReadDetailsTask()
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

TaskContainerInterfacePtr Channel::addRangeDataTasks(TaskContainerInterfacePtr taskContainer,
                                                     const QString &rangeName,
                                                     std::shared_ptr<Range> newRange)
{
    taskContainer->addSub(TaskRangeGetIsAvailable::create(
        m_pcbInterface, m_channelMName, rangeName, newRange->m_available,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range availability: Channel %1 / Range %2").arg(m_channelMName, rangeName)); }));
    taskContainer->addSub(TaskRangeGetUrValue::create(
        m_pcbInterface, m_channelMName, rangeName, newRange->m_urValue,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range UrValue: Channel %1 / Range %2").arg(m_channelMName, rangeName)); }));

    // TODO: add more range tasks

    return taskContainer;
}

TaskTemplatePtr Channel::getRangesRegisterChangeNotificationTask()
{
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer,
            this, &Channel::onInterfaceAnswer);
    constexpr int dontCareNotifyId = 1;
    // TODO: Handle / check unregister
    return TaskChannelRegisterNotifier::create(
        m_pcbInterface, m_channelMName, dontCareNotifyId,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register notification for channel %1").arg(m_channelMName)); });
}

TaskTemplatePtr Channel::getFetchFinalTask()
{
    return TaskLambdaRunner::create([&]() {
        setAvailableRanges();
        emit sigFetchComplete(m_channelMName);
        return true;
    });
}

void Channel::setAvailableRanges()
{
    for(const QString &rangeName : qAsConst(m_allRangeNamesOrderedByServer))
        if(m_rangeNameToRange[rangeName]->m_available)
            m_availableRangeNames.append(rangeName);
}

void Channel::notifyError(QString errMsg)
{
    qWarning("Channel error: %s", qPrintable(errMsg));
}

}

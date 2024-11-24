#include "channelbserver.h"
#include "taskrangegeturvalue.h"
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

void ChannelObserver::startFetch()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());
    m_currentTasks->addSub(getChannelReadDetailsTask());
    m_currentTasks->addSub(getAllRangesTask());
    m_currentTasks->addSub(getRangesRegisterChangeNotificationTask());
    m_currentTasks->addSub(getReadRangeFinalTask());
    m_currentTasks->start();
}

const QStringList ChannelObserver::getRangeNames() const
{
    return m_rangesNamesOrderedByServer;
}

const std::shared_ptr<ChannelObserver::RangeData> ChannelObserver::getRangeData(const QString &rangeName) const
{
    auto iter = m_rangeNameToRangeData.constFind(rangeName);
    if(iter != m_rangeNameToRangeData.constEnd())
        return iter.value();
    qWarning("Range data for range %s not found!", qPrintable(rangeName));
    return nullptr;
}

void ChannelObserver::onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    // TODO?: Remove all decoding: We expect nothing but our notification - but howto test?
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

TaskTemplatePtr ChannelObserver::getAllRangesTask()
{
    TaskContainerInterfacePtr task = TaskContainerSequence::create();
    task->addSub(TaskChannelGetRangeList::create(m_pcbInterface, m_channelMName, m_rangesNamesOrderedByServer,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read range list for channel %1").arg(m_channelMName)); }));
    task->addSub(TaskLambdaRunner::create([&]() {
        TaskContainerInterfacePtr allRangesDetailsTasks = TaskContainerParallel::create();
        for(const QString &rangeName : qAsConst(m_rangesNamesOrderedByServer)) {
            std::shared_ptr<RangeData> newRange = std::make_shared<RangeData>();
            m_rangeNameToRangeData[rangeName] = newRange;
            allRangesDetailsTasks->addSub(TaskRangeGetUrValue::create(
                m_pcbInterface, m_channelMName, rangeName, newRange->m_urValue,
                TRANSACTION_TIMEOUT,
                [&]{ notifyError(QString("Could not range UrValue  fir channel %1 / range %2").arg(m_channelMName, rangeName)); }));

            // TODO: add range tasks
        }
        m_currentTasks->addSub(std::move(allRangesDetailsTasks));
        return true;
    }));
    return task;
}

TaskTemplatePtr ChannelObserver::getRangesRegisterChangeNotificationTask()
{
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer,
            this, &ChannelObserver::onInterfaceAnswer);
    constexpr int dontCareNotifyId = 1;
    // TODO: Handle / check unregister
    return TaskChannelRegisterNotifier::create(
        m_pcbInterface, m_channelMName, dontCareNotifyId,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register notification for channel %1").arg(m_channelMName)); });
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

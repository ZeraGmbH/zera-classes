#include "cro_channel.h"
#include "cro_rangefetchtask.h"
#include "taskserverconnectionstart.h"
#include "taskchannelgetalias.h"
#include "taskchannelgetdspchannel.h"
#include "taskchannelgetunit.h"
#include "taskregisternotifier.h"
#include "taskchannelgetrangelist.h"
#include <taskcontainersequence.h>
#include <taskcontainerparallel.h>
#include <tasklambdarunner.h>
#include <proxy.h>

namespace ChannelRangeObserver {

Channel::Channel(const QString &channelMName,
                 const NetworkConnectionInfo &netInfo,
                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_channelMName(channelMName),
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory))
{
}

void Channel::startFetch()
{
    clearRanges();
    preparePcbInterface();
    startAllRangesTasks();
}

const QString Channel::getMName() const
{
    return m_channelMName;
}

const QStringList Channel::getAllRangeNames() const
{
    return m_allRangeNamesOrderedByServer;
}

const QStringList Channel::getAvailRangeNames() const
{
    return m_availableRangeNames;
}

const RangePtr Channel::getRange(const QString &rangeName) const
{
    auto iter = m_rangeNameToRange.constFind(rangeName);
    if(iter != m_rangeNameToRange.constEnd())
        return iter.value();
    qWarning("Range data for range %s not found!", qPrintable(rangeName));
    return nullptr;
}

void Channel::clearRanges()
{
    m_allRangeNamesOrderedByServer.clear();
    m_availableRangeNames.clear();
    m_rangeNameToRange.clear();
}

void Channel::preparePcbInterface()
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer,
            this, &Channel::onInterfaceAnswer);
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
            RangePtr newRange = std::make_shared<Range>(m_channelMName, rangeName, m_netInfo, m_tcpFactory);
            m_rangeNameToRange[rangeName] = newRange;
            allRangesTasks->addSub(RangeFetchTask::create(newRange));
        }
        m_currentTasks->addSub(std::move(allRangesTasks));
        m_currentTasks->addSub(getRangesRegisterChangeNotificationTask());
        m_currentTasks->addSub(getFetchFinalTask());
        return true;
    }));
    m_currentTasks->addSub(std::move(task));
    connect(m_currentTasks.get(), &TaskTemplate::sigFinish, this, [&](bool ok) {
        m_valid = ok;
        emit sigFetchComplete(m_channelMName, ok);
    });
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

static constexpr int notifyId = 1;
static const char* notificationStr = "Notify:1";

TaskTemplatePtr Channel::getRangesRegisterChangeNotificationTask()
{
    return TaskRegisterNotifier::create(
        m_pcbInterface, QString("SENS:%1:RANG:CAT?").arg(m_channelMName), notifyId,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register notification for channel %1").arg(m_channelMName)); });
}

void Channel::onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(reply)
    if (msgnr == 0 && answer == notificationStr)
        startFetch();
}

TaskTemplatePtr Channel::getFetchFinalTask()
{
    return TaskLambdaRunner::create([&]() {
        setAvailableRanges();
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

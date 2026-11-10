#include "cro_channel.h"
#include "cro_rangefetchtask.h"
#include "taskserverconnectionstart.h"
#include "taskchannelgetalias.h"
#include "taskchannelgetdspchannel.h"
#include "taskchannelgetunit.h"
#include "taskregisternotifier.h"
#include "taskregisternotifierwithvalue.h"
#include "taskchannelgetrangelist.h"
#include <taskcontainersequence.h>
#include <taskcontainerparallel.h>
#include <tasklambdarunner.h>
#include <proxy.h>

namespace ChannelRangeObserver {

Channel::Channel(const QString &channelMName,
                 const NetworkConnectionInfo &netInfo,
                 const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpFactory,
                 bool hasInternalSourceGenerator) :
    m_channelMName(channelMName),
    m_hasInternalSourceGenerator(hasInternalSourceGenerator),
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory)
{
}

void Channel::startFetch()
{
    preparePcbInterface();
    startAllRangesTasks();
}

const QString &Channel::getMName() const
{
    return m_channelMName;
}

const QStringList &Channel::getAllRangeAliases() const
{
    return m_rangesCurr.getAllRangeAliases();
}

const QStringList &Channel::getAvailRangeAliases() const
{
    return m_rangesCurr.getAvailableRangeAliases();
}

const RangePtr Channel::getRange(const QString &rangeAlias) const
{
    RangePtr rangeFound = m_rangesCurr.findRangeByAlias(rangeAlias);
    if (rangeFound != nullptr)
        return rangeFound;
    qCritical("Range data for range %s not found! Available: %s",
              qPrintable(rangeAlias), qPrintable(m_rangesCurr.getAvailableRangeAliases().join(" / ")));
    return std::make_shared<Range>(m_channelMName, "", m_netInfo, m_tcpFactory);
}

ChannelCommonStorage *Channel::getModuleCommonStorage()
{
    return &m_channelAdjStorage;
}

const QString &Channel::getAlias() const
{
    return *m_alias;
}

const QString &Channel::getUnit() const
{
    return *m_unit;
}

int Channel::getDspChannel() const
{
    return *m_dspChannel;
}

bool Channel::isSourceModeOn() const
{
    return m_sourceModeOn;
}

void Channel::preparePcbInterface()
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSuperSmart(m_netInfo, m_tcpFactory);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
            this, &Channel::onInterfaceAnswer);
}

TaskTemplatePtr Channel::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbInterface->getClientSmart(), CONNECTION_TIMEOUT);
}

void Channel::startAllRangesTasks()
{
    m_currentTasks = TaskContainerSequence::create();
    TaskContainerInterfacePtr task = TaskContainerSequence::create();

    task->addSub(getPcbConnectionTask());

    m_allRangeNames = std::make_shared<QStringList>();
    task->addSub(TaskChannelGetRangeList::create(
        m_pcbInterface,
        m_channelMName, m_allRangeNames,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read range list for channel %1").arg(m_channelMName)); }));

    task->addSub(TaskLambdaRunner::create([&]() {
        TaskContainerInterfacePtr allRangesTasks = TaskContainerParallel::create();
        allRangesTasks->addSub(getChannelReadDetailsTask());
        const QStringList &allRangeNames = *m_allRangeNames;
        for (const QString &rangeName : allRangeNames) {
            RangePtr newRange = std::make_shared<Range>(m_channelMName, rangeName, m_netInfo, m_tcpFactory);
            m_rangesNext.addMaidenRange(newRange);
            allRangesTasks->addSub(RangeFetchTask::create(newRange));
        }
        m_currentTasks->addSub(std::move(allRangesTasks));
        m_currentTasks->addSub(getRangesRegisterChangeNotificationTask());
        return true;
    }));
    m_currentTasks->addSub(std::move(task));

    connect(m_currentTasks.get(), &TaskTemplate::sigFinish, this, [&](bool ok) {
        doAfterTaskJobs(ok);
    });
    m_currentTasks->start();
}

void Channel::doAfterTaskJobs(bool ok)
{
    if (ok) {
        m_rangesNext.doAllRangesCompleteJobs();
        m_rangesCurr = m_rangesNext;
    }
    m_rangesNext.clear();
    emit sigNewRanges(m_channelMName, ok);
}

TaskTemplatePtr Channel::getChannelReadDetailsTask()
{
    TaskContainerInterfacePtr task = TaskContainerParallel::create();
    task->addSub(TaskChannelGetAlias::create(
        m_pcbInterface,
        m_channelMName, m_alias,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read alias for channel %1").arg(m_channelMName)); }));
    task->addSub(TaskChannelGetDspChannel::create(
        m_pcbInterface,
        m_channelMName, m_dspChannel,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read dsp-channel for channel %1").arg(m_channelMName)); }));
    task->addSub(TaskChannelGetUnit::create(
        m_pcbInterface,
        m_channelMName, m_unit,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read unit for channel %1").arg(m_channelMName)); }));
    return task;
}

static constexpr int notifyIdRangeChange = 1;
static const char* notificationStrRangeChange = "Notify:1";
static constexpr int notifyIdSourceModeChange = 2;
static const char* notificationStrSourceModeChange = "Notify:2:";

TaskTemplatePtr Channel::getRangesRegisterChangeNotificationTask()
{
    TaskContainerInterfacePtr task = TaskContainerSequence::create();
    task->addSub(TaskRegisterNotifier::create(
        m_pcbInterface,
        QString("SENS:%1:RANG:CAT?").arg(m_channelMName), notifyIdRangeChange,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register range change notification for channel %1").arg(m_channelMName)); }));
    if (m_hasInternalSourceGenerator)
        task->addSub(TaskRegisterNotifierWithValue::create(
            m_pcbInterface,
            QString("GENERATOR:MODEON?"), notifyIdSourceModeChange,
            TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register source mode notification for channel %1").arg(m_channelMName)); }));
    return task;
}

void Channel::onInterfaceAnswer(quint32 msgnr, quint8 reply, const QVariant &answer)
{
    Q_UNUSED(reply)
    if (msgnr == 0) {
        const QString strAnswer = answer.toString();
        if (strAnswer == notificationStrRangeChange)
            startFetch();
        else if (strAnswer.startsWith(notificationStrSourceModeChange)) {
            QString channelMNames = strAnswer;
            channelMNames.remove(notificationStrSourceModeChange);
            const QStringList channelMNameList = channelMNames.split(",");
            bool sourceModeOn = channelMNameList.contains(getMName());
            if (m_sourceModeOn != sourceModeOn) {
                m_sourceModeOn = sourceModeOn;
                emit sigSourceModeOnChanged();
            }
        }
    }
}

void Channel::notifyError(const QString &errMsg)
{
    qWarning("Channel error: %s", qPrintable(errMsg));
}

}

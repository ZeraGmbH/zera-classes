#include "rangeobserver.h"
#include <taskchannelgetrangelist.h>
#include <taskcontainersequence.h>
#include <taskcontainerparallel.h>
#include <tasklambdarunner.h>
#include <taskchannelregisternotifier.h>
#include <proxy.h>

RangeObserver::RangeObserver(const QString &channelMName,
                             const NetworkConnectionInfo &netInfo,
                             VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_channelMName(channelMName),
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{
    Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory);
    m_pcbInterface->setClientSmart(pcbClient);
}

const QStringList RangeObserver::getRangeNames() const
{
    return m_tempRangesNames;
}

void RangeObserver::startReadRanges()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks = addRangesFetchTasks(std::move(m_currentTasks));
    m_currentTasks->start();
}

void RangeObserver::onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        const QStringList answerParts = answer.toString().split(":", Qt::SkipEmptyParts);
        if(answerParts.size() == 2 && answerParts[0] == "Notify")
            startReadRanges();
        else
            qInfo("onInterfaceAnswer: Unknown notification: %s!", qPrintable(answer.toString()));
    }
}

TaskContainerInterfacePtr RangeObserver::addRangesFetchTasks(TaskContainerInterfacePtr tasks)
{
    tasks->addSub(getReadRangeNamesTask());
    // TODO: Handle / check unregister
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer,
            this, &RangeObserver::onInterfaceAnswer);
    tasks->addSub(TaskChannelRegisterNotifier::create(
        m_pcbInterface, m_channelMName, 1,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register notification for channel %1").arg(m_channelMName)); }));

    tasks->addSub(getReadRangeDetailsTasks());

    tasks->addSub(getReadRangeFinalTask());
    return tasks;
}

TaskTemplatePtr RangeObserver::getReadRangeNamesTask()
{
    return TaskChannelGetRangeList::create(
        m_pcbInterface, m_channelMName, m_tempRangesNames,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read range list for channel %1").arg(m_channelMName)); });
}

TaskTemplatePtr RangeObserver::getReadRangeDetailsTasks()
{
    TaskContainerInterfacePtr taskToAdd = TaskContainerParallel::create();

    // TODO: add range tasks

    return taskToAdd;
}

TaskTemplatePtr RangeObserver::getReadRangeFinalTask()
{
    return TaskLambdaRunner::create([=]() {
        emit sigRangeListChanged(m_channelMName);
        return true;
    });
}

void RangeObserver::notifyError(QString errMsg)
{
    qWarning("RangeObserver error: %s", qPrintable(errMsg));
}

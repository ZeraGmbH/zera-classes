#include "range.h"
#include <taskcontainersequence.h>
#include <tasklambdarunner.h>
#include <taskserverconnectionstart.h>
#include <taskrangegetisavailable.h>
#include <taskrangegeturvalue.h>
#include <taskcontainerparallel.h>
#include <proxy.h>

namespace ChannelRangeObserver {

Range::Range(const QString &channelMName, const QString &rangeName,
             const NetworkConnectionInfo &netInfo,
             VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_channelMName(channelMName),
    m_rangeName(rangeName),
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory)),
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{
}

void Range::startFetch()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());

    TaskContainerInterfacePtr rangesTasks = TaskContainerParallel::create();
    rangesTasks->addSub(TaskRangeGetIsAvailable::create(
        m_pcbInterface, m_channelMName, m_rangeName, m_available,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range availability: Channel %1 / Range %2").arg(m_channelMName, m_rangeName)); }));
    rangesTasks->addSub(TaskRangeGetUrValue::create(
        m_pcbInterface, m_channelMName, m_rangeName, m_urValue,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range UrValue: Channel %1 / Range %2").arg(m_channelMName, m_rangeName)); }));
    // TODO: add more range tasks

    m_currentTasks->addSub(std::move(rangesTasks));
    m_currentTasks->addSub(getFetchFinalTask());
}

TaskTemplatePtr Range::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

TaskTemplatePtr Range::getFetchFinalTask()
{
    return TaskLambdaRunner::create([&]() {
        emit sigFetchComplete(m_channelMName, m_rangeName);
        return true;
    });
}

void Range::notifyError(QString errMsg)
{
    qWarning("Range error: %s", qPrintable(errMsg));
}

}

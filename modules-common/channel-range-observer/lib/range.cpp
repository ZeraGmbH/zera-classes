#include "range.h"
#include <taskcontainersequence.h>
#include <tasklambdarunner.h>
#include <taskserverconnectionstart.h>
#include <proxy.h>

namespace ChannelRangeObserver {

Range::Range(const QString &channelMName,
             const NetworkConnectionInfo &netInfo,
             VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_channelMName(channelMName),
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory)),
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{

}

void Range::startFetch()
{
    TaskContainerInterfacePtr task = TaskContainerSequence::create();
    task->addSub(getPcbConnectionTask());
    /*task->addSub(TaskLambdaRunner::create([&]() {
        taskContainer->addSub(TaskRangeGetIsAvailable::create(
            m_pcbInterface, m_channelMName, rangeName, newRange->m_available,
            TRANSACTION_TIMEOUT,
            [&]{ notifyError(QString("Could not fetch range availability: Channel %1 / Range %2").arg(m_channelMName, rangeName)); }));
        taskContainer->addSub(TaskRangeGetUrValue::create(
            m_pcbInterface, m_channelMName, rangeName, newRange->m_urValue,
            TRANSACTION_TIMEOUT,
            [&]{ notifyError(QString("Could not fetch range UrValue: Channel %1 / Range %2").arg(m_channelMName, rangeName)); }));

        // TODO: add more range tasks

        return true;
    }));*/
}

TaskTemplatePtr Range::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

}

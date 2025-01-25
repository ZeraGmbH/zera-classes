#include "cro_range.h"
#include "taskserverconnectionstart.h"
#include "taskrangegetisavailable.h"
#include "taskrangegettype.h"
#include "taskrangegeturvalue.h"
#include "taskrangegetrejection.h"
#include "taskrangegetovrejection.h"
#include "taskrangegetadcrejection.h"
#include <taskcontainersequence.h>
#include <taskcontainerparallel.h>
#include <tasklambdarunner.h>
#include <proxy.h>

namespace ChannelRangeObserver {

Range::Range(const QString &channelMName, const QString &rangeName,
             const NetworkConnectionInfo &netInfo,
             VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_channelMName(channelMName),
    m_rangeName(rangeName),
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory))
{
}

void Range::startFetch()
{
    preparePcbInterface();
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());

    TaskContainerInterfacePtr rangesTasks = TaskContainerParallel::create();
    rangesTasks->addSub(TaskRangeGetIsAvailable::create(
        m_pcbInterface, m_channelMName, m_rangeName, m_available,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range availability: Channel %1 / Range %2").arg(m_channelMName, m_rangeName)); }));
    rangesTasks->addSub(TaskRangeGetType::create(
        m_pcbInterface, m_channelMName, m_rangeName, m_type,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range type: Channel %1 / Range %2").arg(m_channelMName, m_rangeName)); }));
    rangesTasks->addSub(TaskRangeGetUrValue::create(
        m_pcbInterface, m_channelMName, m_rangeName, m_urValue,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range UrValue: Channel %1 / Range %2").arg(m_channelMName, m_rangeName)); }));
    rangesTasks->addSub(TaskRangeGetRejection::create(
        m_pcbInterface, m_channelMName, m_rangeName, m_rejection,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range rejection: Channel %1 / Range %2").arg(m_channelMName, m_rangeName)); }));
    rangesTasks->addSub(TaskRangeGetOvRejection::create(
        m_pcbInterface, m_channelMName, m_rangeName, m_ovrejection,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range ovrejection: Channel %1 / Range %2").arg(m_channelMName, m_rangeName)); }));
    rangesTasks->addSub(TaskRangeGetAdcRejection::create(
        m_pcbInterface, m_channelMName, m_rangeName, m_adcrejection,
        TRANSACTION_TIMEOUT,
        [&]{ notifyError(QString("Could not fetch range adcrejection: Channel %1 / Range %2").arg(m_channelMName, m_rangeName)); }));

    // TODO: add more range tasks

    m_currentTasks->addSub(std::move(rangesTasks));
    connect(m_currentTasks.get(), &TaskTemplate::sigFinish, this, [&](bool ok) {
        emit sigFetchDoneRange(m_channelMName, m_rangeName, ok);
    });
    m_currentTasks->start();
}

void Range::preparePcbInterface()
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
}

TaskTemplatePtr Range::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

void Range::notifyError(QString errMsg)
{
    qWarning("Range error: %s", qPrintable(errMsg));
}

}

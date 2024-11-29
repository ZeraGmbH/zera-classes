#include "cro_channelcurrentrange.h"
#include "taskserverconnectionstart.h"
#include "taskchannelgetcurrentrange.h"
#include "taskregisternotifier.h"
#include <taskcontainersequence.h>
#include <proxy.h>

namespace ChannelRangeObserver {

ChannelCurrentRange::ChannelCurrentRange(const QString &channelMName,
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

const QString ChannelCurrentRange::getCurrentRange() const
{
    return m_currentRangeName;
}

static constexpr int notifyId = 1;
static const char* notificationStr = "Notify:1";

void ChannelCurrentRange::startObserve()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());
    m_currentTasks->addSub(TaskChannelGetCurrentRange::create(
        m_pcbInterface, m_channelMName, m_currentRangeName,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read dsp-channel for channel %1").arg(m_channelMName)); }));
    m_currentTasks->addSub(
        TaskRegisterNotifier::create(m_pcbInterface, QString("SENS:%1:RANG?").arg(m_channelMName), notifyId,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not register notification for channel %1").arg(m_channelMName)); }));
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer,
            this, &ChannelCurrentRange::onInterfaceAnswer);
    connect(m_currentTasks.get(), &TaskTemplate::sigFinish, this, [&](bool ok) {
        emit sigFetchComplete(m_channelMName, m_currentRangeName, ok);
    });
    m_currentTasks->start();
}

void ChannelCurrentRange::onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(reply)
    if(msgnr == 0 && answer == notificationStr) {
        m_currentTasks->addSub(TaskChannelGetCurrentRange::create(
            m_pcbInterface,m_channelMName, m_currentRangeName,
            TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read dsp-channel for channel %1").arg(m_channelMName)); }));
        m_currentTasks->start();
    }
}

TaskTemplatePtr ChannelCurrentRange::getPcbConnectionTask()
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

void ChannelCurrentRange::notifyError(QString errMsg)
{
    qWarning("Channel current range observer error: %s", qPrintable(errMsg));
}

}

#include "cro_channelcurrentrange.h"
#include "taskserverconnectionstart.h"
#include "taskchannelgetcurrentrange.h"
#include "taskregisternotifierwithvalue.h"
#include <taskcontainersequence.h>
#include <proxy.h>

namespace ChannelRangeObserver {

ChannelCurrentRange::ChannelCurrentRange(const QString &channelMName,
                                         const NetworkConnectionInfo &netInfo,
                                         VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_channelMName(channelMName),
    m_netInfo(netInfo),
    m_tcpFactory(tcpFactory),
    m_pcbClient(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpFactory))
{
}

const QString ChannelCurrentRange::getCurrentRange() const
{
    return m_currentRangeName;
}

void ChannelCurrentRange::startObserve()
{
    preparePcbInterface();
    startTasks();
}

void ChannelRangeObserver::ChannelCurrentRange::preparePcbInterface()
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
            this, &ChannelCurrentRange::onInterfaceAnswer);
}

static constexpr int notifyId = 1;
static const char* notificationStr = "Notify:1";

void ChannelRangeObserver::ChannelCurrentRange::startTasks()
{
    m_currentTasks = TaskContainerSequence::create();
    m_currentTasks->addSub(getPcbConnectionTask());
    m_currentTasks->addSub(TaskChannelGetCurrentRange::create(
        m_pcbInterface, m_channelMName, m_currentRangeName,
        TRANSACTION_TIMEOUT, [=]{ notifyError(QString("Could not read current range for channel %1").arg(m_channelMName)); }));
    m_currentTasks->addSub(TaskRegisterNotifierWithValue::create(
        m_pcbInterface, QString("SENS:%1:RANG?").arg(m_channelMName), notifyId,
        TRANSACTION_TIMEOUT, [=]{ notifyError(QString("Could not register notification for channel %1").arg(m_channelMName)); }));
    connect(m_currentTasks.get(), &TaskTemplate::sigFinish, this, [&](bool ok) {
        emit sigFetchDoneCurrentRange(m_channelMName, m_currentRangeName, ok);
        m_currentTasks = nullptr;
    });
    m_currentTasks->start();
}

void ChannelCurrentRange::onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(reply)
    if(msgnr == 0) {
        QStringList entries = answer.toString().split(":");
        if(answer.toString().contains(notificationStr) && entries.count() == 3) {
            m_currentRangeName = entries[2];
            emit sigFetchDoneCurrentRange(m_channelMName, m_currentRangeName, true);
        }
        else
            notifyError("Unkown notification");
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

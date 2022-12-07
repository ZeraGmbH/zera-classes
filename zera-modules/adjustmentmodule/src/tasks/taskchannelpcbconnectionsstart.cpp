#include "taskchannelpcbconnectionsstart.h"
#include "taskserverconnectionstart.h"
#include "tasktimeoutdecorator.h"
#include "adjustmentmodulemeasprogram.h"

namespace ADJUSTMENTMODULE {

std::unique_ptr<TaskComposite> TaskChannelPcbConnectionsStart::create(AdjustmentModuleActivateDataPtr activateData,
        QStringList channels,
        QString pcbServerIp,
        int connectTimeout)
{
    return std::make_unique<TaskChannelPcbConnectionsStart>(activateData, channels, pcbServerIp, connectTimeout);
}

TaskChannelPcbConnectionsStart::TaskChannelPcbConnectionsStart(AdjustmentModuleActivateDataPtr activateData,
                                                               QStringList channels,
                                                               QString pcbServerIp,
                                                               int connectTimeout) :
    m_activationData(activateData),
    m_channels(channels),
    m_pcbServerIp(pcbServerIp),
    m_connectTimeout(connectTimeout)
{
}

void TaskChannelPcbConnectionsStart::start()
{
    for(const QString& channelName : qAsConst(m_channels)) {
        cAdjustChannelInfo* adjustChannelInfo = new cAdjustChannelInfo();
        m_activationData->m_adjustChannelInfoHash[channelName] = adjustChannelInfo;
        int port = m_activationData->m_chnPortHash[channelName];
        if (m_activationData->m_portChannelHash.contains(port)) {
            // the channels share the same interface
            adjustChannelInfo->m_pPCBInterface = m_activationData->m_adjustChannelInfoHash[m_activationData->m_portChannelHash[port] ]->m_pPCBInterface;
        }
        else {
            m_activationData->m_portChannelHash[port] = channelName;
            Zera::Proxy::ProxyClientPtr pcbclient = Zera::Proxy::cProxy::getInstance()->getConnectionSmart(m_pcbServerIp, port);
            m_activationData->m_pcbClientList.append(pcbclient);

            Zera::Server::cPCBInterface *pcbInterface = new Zera::Server::cPCBInterface();
            m_activationData->m_pcbInterfaceList.append(pcbInterface);
            pcbInterface->setClientSmart(pcbclient);
            adjustChannelInfo->m_pPCBInterface = pcbInterface;
            m_pcbActivationTasks.addTask(TaskTimeoutDecorator::wrapTimeout(m_connectTimeout,
                                                                           TaskServerConnectionStart::create(pcbclient)));
        }
    }
    connect(&m_pcbActivationTasks, &TaskParallel::sigFinish, this, &TaskChannelPcbConnectionsStart::sigFinish);
    m_pcbActivationTasks.start();
}

}

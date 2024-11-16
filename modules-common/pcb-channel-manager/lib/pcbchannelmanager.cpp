#include "pcbchannelmanager.h"
#include "taskchannelgetavail.h"
#include "taskserverconnectionstart.h"
#include <taskcontainersequence.h>

void PcbChannelManager::startScan(Zera::ProxyClientPtr pcbClient)
{
    if(m_channelMNames.isEmpty()) {
        createTasks(pcbClient);
        connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
                this, &PcbChannelManager::onTasksFinish);
        m_currentTasks->start();
    }
    else
        emit sigScanFinished(true);
}

QStringList PcbChannelManager::getChannelMNames() const
{
    return m_channelMNames;
}

void PcbChannelManager::onTasksFinish(bool ok)
{
    m_currentTasks = nullptr;
    emit sigScanFinished(ok);
}

void PcbChannelManager::createTasks(Zera::ProxyClientPtr pcbClient)
{
    m_currentTasks = TaskContainerSequence::create();

    m_currentTasks->addSub(TaskServerConnectionStart::create(pcbClient, CONNECTION_TIMEOUT));

    Zera::PcbInterfacePtr pcbInterface = std::make_shared<Zera::cPCBInterface>();
    pcbInterface->setClientSmart(pcbClient);
    m_currentTasks->addSub(TaskChannelGetAvail::create(pcbInterface, m_channelMNames, TRANSACTION_TIMEOUT));
}

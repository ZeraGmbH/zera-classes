#include "pcbchannelmanager.h"
#include "taskchannelgetavail.h"
#include <abstractserverInterface.h>
#include <taskserverconnectionstart.h>
#include <taskcontainersequence.h>

PcbChannelManager::PcbChannelManager()
{
}

void PcbChannelManager::startScan(Zera::ProxyClientPtr pcbClient)
{
    m_currentTasks = TaskContainerSequence::create();

    m_currentTasks->addSub(TaskServerConnectionStart::create(pcbClient, CONNECTION_TIMEOUT));

    Zera::PcbInterfacePtr pcbInterface = std::make_shared<Zera::cPCBInterface>();
    pcbInterface->setClientSmart(pcbClient);
    m_currentTasks->addSub(TaskChannelGetAvail::create(pcbInterface, m_channelNames, TRANSACTION_TIMEOUT));

    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &PcbChannelManager::sigScanFinished);
    m_currentTasks->start();
}

QStringList PcbChannelManager::getChannelMNames() const
{
    return m_channelNames;
}

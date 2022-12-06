#include "taskrmconnectionopen.h"
#include "adjustmentmoduleconfiguration.h"
#include "proxy.h"
#include "proxyclient.h"

namespace ADJUSTMENTMODULE {

std::unique_ptr<TaskRmConnectionOpen> TaskRmConnectionOpen::create(Zera::Server::RMInterfacePtr rmInterface, Zera::Proxy::ProxyClientPtr rmClient)
{
    return std::make_unique<TaskRmConnectionOpen>(rmInterface, rmClient);
}

TaskRmConnectionOpen::TaskRmConnectionOpen(Zera::Server::RMInterfacePtr rmInterface, Zera::Proxy::ProxyClientPtr rmClient) :
    m_rmInterface(rmInterface),
    m_rmClient(rmClient)
{
}

void TaskRmConnectionOpen::start()
{
    connect(m_rmClient.get(), &Zera::Proxy::cProxyClient::connected, this, &TaskRmConnectionOpen::onConnected);
    m_rmInterface->setClientSmart(m_rmClient);
    Zera::Proxy::cProxy::getInstance()->startConnectionSmart(m_rmClient);
}

void TaskRmConnectionOpen::onConnected()
{
    emit sigFinish(true, getTaskId());
}

}

#include "taskrmconnectionstart.h"
#include "proxy.h"

namespace ADJUSTMENTMODULE {

std::unique_ptr<TaskRmConnectionStart> TaskRmConnectionStart::create(Zera::Proxy::ProxyClientPtr rmClient)
{
    return std::make_unique<TaskRmConnectionStart>(rmClient);
}

TaskRmConnectionStart::TaskRmConnectionStart(Zera::Proxy::ProxyClientPtr rmClient) :
    m_rmClient(rmClient)
{
}

void TaskRmConnectionStart::start()
{
    connect(m_rmClient.get(), &Zera::Proxy::cProxyClient::connected, this, &TaskRmConnectionStart::onConnected);
    Zera::Proxy::cProxy::getInstance()->startConnectionSmart(m_rmClient);
}

void TaskRmConnectionStart::onConnected()
{
    finishTask(true);
}

}

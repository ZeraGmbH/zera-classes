#include "taskserverconnectionstart.h"
#include "proxy.h"

std::unique_ptr<TaskComposite> TaskServerConnectionStart::create(Zera::Proxy::ProxyClientPtr client)
{
    return std::make_unique<TaskServerConnectionStart>(client);
}

TaskServerConnectionStart::TaskServerConnectionStart(Zera::Proxy::ProxyClientPtr client) :
    m_client(client)
{
}

void TaskServerConnectionStart::start()
{
    connect(m_client.get(), &Zera::Proxy::cProxyClient::connected, this, &TaskServerConnectionStart::onConnected);
    Zera::Proxy::cProxy::getInstance()->startConnectionSmart(m_client);
}

void TaskServerConnectionStart::onConnected()
{
    finishTask(true);
}


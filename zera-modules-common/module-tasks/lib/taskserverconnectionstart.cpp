#include "taskserverconnectionstart.h"
#include "proxy.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskServerConnectionStart::create(Zera::ProxyClientPtr client, int timeout)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout, std::make_unique<TaskServerConnectionStart>(client));
}

TaskServerConnectionStart::TaskServerConnectionStart(Zera::ProxyClientPtr client) :
    m_client(client)
{
}

void TaskServerConnectionStart::start()
{
    connect(m_client.get(), &Zera::ProxyClient::connected,
            this, &TaskServerConnectionStart::onConnected);
    Zera::Proxy::getInstance()->startConnectionSmart(m_client);
}

void TaskServerConnectionStart::onConnected()
{
    finishTask(true);
}


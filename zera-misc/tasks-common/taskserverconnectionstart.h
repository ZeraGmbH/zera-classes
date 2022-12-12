#ifndef TASKSERVERCONNECTIONSTART_H
#define TASKSERVERCONNECTIONSTART_H

#include "taskcomposit.h"
#include "proxyclient.h"

class TaskServerConnectionStart : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(Zera::Proxy::ProxyClientPtr client);
    static std::unique_ptr<TaskComposite> create(Zera::Proxy::ProxyClientPtr client, int timeout);

    TaskServerConnectionStart(Zera::Proxy::ProxyClientPtr client);
    void start() override;

private slots:
    void onConnected();
private:
    Zera::Proxy::ProxyClientPtr m_client;
};

#endif // TASKSERVERCONNECTIONSTART_H

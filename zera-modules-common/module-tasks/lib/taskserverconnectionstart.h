#ifndef TASKSERVERCONNECTIONSTART_H
#define TASKSERVERCONNECTIONSTART_H

#include "tasktemplate.h"
#include "proxyclient.h"

class TaskServerConnectionStart : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::Proxy::ProxyClientPtr client, int timeout);
    TaskServerConnectionStart(Zera::Proxy::ProxyClientPtr client);
    void start() override;
private slots:
    void onConnected();
private:
    Zera::Proxy::ProxyClientPtr m_client;
};

#endif // TASKSERVERCONNECTIONSTART_H

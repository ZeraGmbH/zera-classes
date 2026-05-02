#ifndef TASKSERVERCONNECTIONSTART_H
#define TASKSERVERCONNECTIONSTART_H

#include "tasktemplate.h"
#include "proxyclient.h"

class TaskServerConnectionStart : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(const Zera::ProxyClientPtr &client, int timeout);
    TaskServerConnectionStart(const Zera::ProxyClientPtr &client);
    void start() override;
private slots:
    void onConnected();
private:
    Zera::ProxyClientPtr m_client;
};

#endif // TASKSERVERCONNECTIONSTART_H

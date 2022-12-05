#ifndef TASKRMCONNECTIONOPEN_H
#define TASKRMCONNECTIONOPEN_H

#include "taskcomposit.h"
#include "basemoduleconfiguration.h"
#include "adjustmentmoduleconfigdata.h"
#include "rminterface.h"

namespace ADJUSTMENTMODULE
{

class TaskRmConnectionOpen : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskRmConnectionOpen> create(Zera::Server::cRMInterface *rmInterface, Zera::Proxy::ProxyClientPtr rmClient);
    TaskRmConnectionOpen(Zera::Server::cRMInterface* rmInterface, Zera::Proxy::ProxyClientPtr rmClient);
    void start() override;

private slots:
    void onConnected();
private:
    Zera::Server::cRMInterface* m_rmInterface;
    Zera::Proxy::ProxyClientPtr m_rmClient;
};
}
#endif // TASKRMCONNECTIONOPEN_H

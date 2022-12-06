#ifndef TASKRMCONNECTIONSTART_H
#define TASKRMCONNECTIONSTART_H

#include "taskcomposit.h"
#include "proxyclient.h"

namespace ADJUSTMENTMODULE {

class TaskRmConnectionStart : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskRmConnectionStart> create(Zera::Proxy::ProxyClientPtr rmClient);
    TaskRmConnectionStart(Zera::Proxy::ProxyClientPtr rmClient);
    void start() override;

private slots:
    void onConnected();
private:
    Zera::Proxy::ProxyClientPtr m_rmClient;
};

}
#endif // TASKRMCONNECTIONSTART_H

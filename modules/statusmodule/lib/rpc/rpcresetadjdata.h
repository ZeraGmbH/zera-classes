#ifndef RPCRESETADJDATA_H
#define RPCRESETADJDATA_H

#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <vf-cpp-rpc-simplified.h>

class RPCResetAdjData : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RPCResetAdjData(Zera::PcbInterfacePtr pcbInterface,
                    VeinEvent::EventSystem *eventSystem,
                    int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcTaskFinish(bool ok, QUuid rpcCallId);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    TaskContainerInterfacePtr m_tasksQueue;
};

#endif // RPCRESETADJDATA_H

#ifndef RPCREADLOCKSTATE_H
#define RPCREADLOCKSTATE_H

#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <vf-cpp-rpc-simplified.h>
#include <QUuid>

class RPCReadLockState : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RPCReadLockState(Zera::PcbInterfacePtr pcbInterface, VeinEvent::EventSystem *eventSystem, int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onTaskFinish(bool ok);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    TaskContainerInterfacePtr m_readLockStateQueue;
    QUuid m_rpcCallId;
    std::shared_ptr<int> m_lockStateReceived;
};

#endif // RPCREADLOCKSTATE_H

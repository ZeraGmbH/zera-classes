#include "rpcreadlockstate.h"
#include "taskemobreadconnectionstate.h"
#include <vf-cpp-rpc-signature.h>

RPCReadLockState::RPCReadLockState(Zera::PcbInterfacePtr pcbInterface, VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                "RPC_readLockState", VfCpp::VfCppRpcSignature::RPCParams({})) ),
    m_pcbInterface(pcbInterface),
    m_lockStateReceived(std::make_shared<int>())
{
}

void RPCReadLockState::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    Q_UNUSED(parameters) // for now
    m_rpcCallId = callId;
    m_readLockStateTask = TaskEmobReadConnectionState::create(m_pcbInterface, m_lockStateReceived, TRANSACTION_TIMEOUT);
    connect(m_readLockStateTask.get(), &TaskTemplate::sigFinish,
            this, &RPCReadLockState::onTaskFinish);
    m_readLockStateTask->start();
}

void RPCReadLockState::onTaskFinish(bool ok)
{
    if(ok)
        sendRpcResult(m_rpcCallId, *m_lockStateReceived);
    else
        sendRpcError(m_rpcCallId, QString::number(Zera::cPCBInterface::emobstate_error));
}

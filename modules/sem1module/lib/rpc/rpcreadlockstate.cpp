#include "rpcreadlockstate.h"
#include <vf-cpp-rpc-signature.h>

RPCReadLockState::RPCReadLockState(VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                "RPC_readLockState", VfCpp::VfCppRpcSignature::RPCParams({})) )
{
}

void RPCReadLockState::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    Q_UNUSED(parameters) // for now
    emit sigReadLockState(callId);
}

void RPCReadLockState::onReadLockStateCompleted(const QUuid &callId, bool success, QVariant errorMsg, QVariant value)
{
    if(success)
        sendRpcResult(callId, value);
    else
        sendRpcError(callId, errorMsg.toString());
}

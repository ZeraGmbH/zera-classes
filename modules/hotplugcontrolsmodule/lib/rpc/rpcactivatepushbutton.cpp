#include "rpcactivatepushbutton.h"
#include "taskemobactivatepushbutton.h"
#include <taskrpctaskwrapper.h>
#include <vf-cpp-rpc-signature.h>
#include <taskcontainerqueue.h>
#include <quuid.h>

RpcActivatePushButton::RpcActivatePushButton(Zera::PcbInterfacePtr pcbInterface, VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                                VfCpp::VfCppRpcSignature::createRpcSignature(
                                    "RPC_activatePushButton", VfCpp::VfCppRpcSignature::RPCParams({}))),
    m_pcbInterface(pcbInterface),
    m_activatePushButtonQueue(TaskContainerQueue::create())
{
}

void RpcActivatePushButton::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    Q_UNUSED(parameters)
    TaskTemplatePtr taskActivePB = TaskEmobActivatePushButton::create(m_pcbInterface, TRANSACTION_TIMEOUT);
    TaskRpcTransactionWrapperPtr taskRpcWrapper = TaskRpcTaskWrapper::create(std::move(taskActivePB), callId);
    connect(taskRpcWrapper.get(), &TaskRpcTaskWrapper::sigRpcFinished, this, &RpcActivatePushButton::onRpcTaskFinish);
    m_activatePushButtonQueue->addSub(std::move(taskRpcWrapper));
}

void RpcActivatePushButton::onRpcTaskFinish(bool ok, QUuid rpcCallId)
{
    if(ok)
        sendRpcResult(rpcCallId, QVariant());
    else
        sendRpcError(rpcCallId, QString());
}

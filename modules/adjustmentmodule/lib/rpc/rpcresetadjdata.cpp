#include "rpcresetadjdata.h"
#include "taskresetadjdata.h"
#include <taskrpctaskwrapper.h>
#include <taskcontainerqueue.h>
#include <vf-cpp-rpc-signature.h>

RPCResetAdjData::RPCResetAdjData(Zera::PcbInterfacePtr pcbInterface, VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                  "RPC_resetAdjustmentData", VfCpp::VfCppRpcSignature::RPCParams({}))),
    m_pcbInterface(pcbInterface),
    m_tasksQueue(TaskContainerQueue::create())
{
}

void RPCResetAdjData::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    TaskTemplatePtr taskReset = TaskResetAdjData::create(m_pcbInterface, TRANSACTION_TIMEOUT);
    TaskRpcTransactionWrapperPtr taskRpcWrapper = TaskRpcTaskWrapper::create(std::move(taskReset), callId);
    connect(taskRpcWrapper.get(), &TaskRpcTaskWrapper::sigRpcFinished, this, &RPCResetAdjData::onRpcTaskFinish);
    m_tasksQueue->addSub(std::move(taskRpcWrapper));
}

void RPCResetAdjData::onRpcTaskFinish(bool ok, QUuid rpcCallId)
{
    if(ok)
        sendRpcResult(rpcCallId, QString("Reset is done successfully"));
    else
        sendRpcError(rpcCallId, QString("Error while resetting data"));
}

#include "rpcclearerror.h"
#include "servicechannelnamehelper.h"
#include "taskemobclearerrorstatus.h"
#include <taskcontainerqueue.h>
#include <taskrpctaskwrapper.h>
#include <vf-cpp-rpc-signature.h>

RpcClearError::RpcClearError(Zera::PcbInterfacePtr pcbInterface,
                                         ChannelRangeObserver::SystemObserverPtr observer,
                                         VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                  "RPC_clearError", VfCpp::VfCppRpcSignature::RPCParams({{"p_channelName", "QString"}}))),
    m_pcbInterface(pcbInterface),
    m_clearErrorQueue(TaskContainerQueue::create()),
    m_observer(observer)
{
}

void RpcClearError::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    QString channelName = parameters["p_channelName"].toString();
    QString channelMName = ServiceChannelNameHelper::getChannelMName(channelName, m_observer);

    TaskTemplatePtr taskReadState = TaskEmobClearErrorStatus::create(
        m_pcbInterface, channelMName, TRANSACTION_TIMEOUT);
    TaskRpcTransactionWrapperPtr taskRpcWrapper = TaskRpcTaskWrapper::create(std::move(taskReadState), callId);
    connect(taskRpcWrapper.get(), &TaskRpcTaskWrapper::sigRpcFinished,
            this, &RpcClearError::onRpcTaskFinish);
    m_clearErrorQueue->addSub(std::move(taskRpcWrapper));
}

void RpcClearError::onRpcTaskFinish(bool ok, QUuid rpcCallId)
{
    if(ok)
        sendRpcResult(rpcCallId, QVariant());
    else
        sendRpcError(rpcCallId, QString());
}

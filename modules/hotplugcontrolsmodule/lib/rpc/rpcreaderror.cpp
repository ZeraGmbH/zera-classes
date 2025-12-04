#include "rpcreaderror.h"
#include "servicechannelnamehelper.h"
#include "taskemobreaderrorstatus.h"
#include <emobdefinitions.h>
#include <taskcontainerqueue.h>
#include <taskrpctaskwrapper.h>
#include <vf-cpp-rpc-signature.h>


RPCReadError::RPCReadError(Zera::PcbInterfacePtr pcbInterface, ChannelRangeObserver::SystemObserverPtr observer,
                             VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                                VfCpp::VfCppRpcSignature::createRpcSignature(
                                    "RPC_readError", VfCpp::VfCppRpcSignature::RPCParams({{"p_channelName", "QString"}}))),
    m_pcbInterface(pcbInterface),
    m_readErrorQueue(TaskContainerQueue::create()),
    m_emobErrorStatus(std::make_shared<int>()),
    m_observer(observer)
{
}

void RPCReadError::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    QString channelName = parameters["p_channelName"].toString();
    QString channelMName = ServiceChannelNameHelper::getChannelMName(channelName, m_observer);

    TaskTemplatePtr taskReadState = TaskEmobReadErrorStatus::create(
        m_pcbInterface, m_emobErrorStatus, channelMName, TRANSACTION_TIMEOUT);
    TaskRpcTransactionWrapperPtr taskRpcWrapper = TaskRpcTaskWrapper::create(std::move(taskReadState), callId);
    connect(taskRpcWrapper.get(), &TaskRpcTaskWrapper::sigRpcFinished,
            this, &RPCReadError::onRpcTaskFinish);
    m_readErrorQueue->addSub(std::move(taskRpcWrapper));
}

void RPCReadError::onRpcTaskFinish(bool ok, QUuid rpcCallId)
{
    if(ok)
        sendRpcResult(rpcCallId, *m_emobErrorStatus);
    else
        sendRpcError(rpcCallId, QString::number(1<<16));
}

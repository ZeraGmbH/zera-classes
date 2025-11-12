#include "rpcreadlockstate.h"
#include "taskemobreadconnectionstate.h"
#include "servicechannelnamehelper.h"
#include <emobdefinitions.h>
#include <taskcontainerqueue.h>
#include <taskrpctaskwrapper.h>
#include <vf-cpp-rpc-signature.h>

RPCReadLockState::RPCReadLockState(Zera::PcbInterfacePtr pcbInterface, ChannelRangeObserver::SystemObserverPtr observer,
                                   VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                "RPC_readLockState", VfCpp::VfCppRpcSignature::RPCParams({{"p_channelName", "QString"}})) ),
    m_pcbInterface(pcbInterface),
    m_readLockStateQueue(TaskContainerQueue::create()),
    m_lockStateReceived(std::make_shared<int>()),
    m_observer(observer)
{
}

void RPCReadLockState::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    QString channelName = parameters["p_channelName"].toString();
    QString channelMName = ServiceChannelNameHelper::getChannelMName(channelName, m_observer);

    TaskTemplatePtr taskReadState = TaskEmobReadConnectionState::create(
        m_pcbInterface, m_lockStateReceived, channelMName, TRANSACTION_TIMEOUT);
    TaskRpcTransactionWrapperPtr taskRpcWrapper = TaskRpcTaskWrapper::create(std::move(taskReadState), callId);
    connect(taskRpcWrapper.get(), &TaskRpcTaskWrapper::sigRpcFinished,
            this, &RPCReadLockState::onRpcTaskFinish);
    m_readLockStateQueue->addSub(std::move(taskRpcWrapper));
}

void RPCReadLockState::onRpcTaskFinish(bool ok, QUuid rpcCallId)
{
    if(ok)
        sendRpcResult(rpcCallId, *m_lockStateReceived);
    else
        sendRpcError(rpcCallId, QString::number(reademoblockstate::emobstate_error));
}

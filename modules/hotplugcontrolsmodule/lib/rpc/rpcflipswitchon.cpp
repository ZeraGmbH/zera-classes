#include "rpcflipswitchon.h"
#include "servicechannelnamehelper.h"
#include "taskemobflipswitch.h"
#include <taskcontainerqueue.h>
#include <taskrpctaskwrapper.h>
#include <vf-cpp-rpc-signature.h>

RpcFlipSwitchOn::RpcFlipSwitchOn(Zera::PcbInterfacePtr pcbInterface,
                             ChannelRangeObserver::SystemObserverPtr observer,
                             VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                  "RPC_flipSwitchOn", VfCpp::VfCppRpcSignature::RPCParams({{"p_channelName", "QString"}}))),
    m_pcbInterface(pcbInterface),
    m_flipSwitchQueue(TaskContainerQueue::create()),
    m_observer(observer)
{
}

void RpcFlipSwitchOn::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    QString channelName = parameters["p_channelName"].toString();
    QString channelMName = ServiceChannelNameHelper::getChannelMName(channelName, m_observer);

    TaskTemplatePtr taskSwitch = TaskEmobFlipSwitch::create(m_pcbInterface, channelMName, true, TRANSACTION_TIMEOUT);
    TaskRpcTransactionWrapperPtr taskRpcWrapper = TaskRpcTaskWrapper::create(std::move(taskSwitch), callId);
    connect(taskRpcWrapper.get(), &TaskRpcTaskWrapper::sigRpcFinished, this, &RpcFlipSwitchOn::onRpcTaskFinish);
    m_flipSwitchQueue->addSub(std::move(taskRpcWrapper));
}

void RpcFlipSwitchOn::onRpcTaskFinish(bool ok, QUuid rpcCallId)
{
    if(ok)
        sendRpcResult(rpcCallId, QVariant());
    else
        sendRpcError(rpcCallId, QString());
}

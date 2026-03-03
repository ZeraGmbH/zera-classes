#include "rpcflipswitchoff.h"
#include "servicechannelnamehelper.h"
#include "taskemobflipswitch.h"
#include <taskcontainerqueue.h>
#include <taskrpctaskwrapper.h>
#include <vf-cpp-rpc-signature.h>

RpcFlipSwitchOff::RpcFlipSwitchOff(Zera::PcbInterfacePtr pcbInterface,
                                   ChannelRangeObserver::SystemObserverPtr observer,
                                   VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                  "RPC_flipSwitchOff", VfCpp::VfCppRpcSignature::RPCParams({{"p_channelName", "QString"}}))),
    m_pcbInterface(pcbInterface),
    m_flipSwitchQueue(TaskContainerQueue::create()),
    m_observer(observer)
{

}

void RpcFlipSwitchOff::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    QString channelName = parameters["p_channelName"].toString();
    QString channelMName = ServiceChannelNameHelper::getChannelMName(channelName, m_observer);

    TaskTemplatePtr taskSwitch = TaskEmobFlipSwitch::create(m_pcbInterface, channelMName, false, TRANSACTION_TIMEOUT);
    TaskRpcTransactionWrapperPtr taskRpcWrapper = TaskRpcTaskWrapper::create(std::move(taskSwitch), callId);
    connect(taskRpcWrapper.get(), &TaskRpcTaskWrapper::sigRpcFinished, this, &RpcFlipSwitchOff::onRpcTaskFinish);
    m_flipSwitchQueue->addSub(std::move(taskRpcWrapper));
}

void RpcFlipSwitchOff::onRpcTaskFinish(bool ok, QUuid rpcCallId)
{
    if(ok)
        sendRpcResult(rpcCallId, QVariant());
    else
        sendRpcError(rpcCallId, QString());
}

#include "rpcdischargeoff.h"
#include "servicechannelnamehelper.h"
#include "taskemobdischarge.h"
#include <taskcontainerqueue.h>
#include <taskrpctaskwrapper.h>
#include <vf-cpp-rpc-signature.h>

RpcDischargeOff::RpcDischargeOff(Zera::PcbInterfacePtr pcbInterface,
                                 ChannelRangeObserver::SystemObserverPtr observer,
                                 VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                  "RPC_dischargeOff", VfCpp::VfCppRpcSignature::RPCParams({{"p_channelName", "QString"}}))),
    m_pcbInterface(pcbInterface),
    m_taskQueue(TaskContainerQueue::create()),
    m_observer(observer)
{

}

void RpcDischargeOff::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    QString channelName = parameters["p_channelName"].toString();
    QString channelMName = ServiceChannelNameHelper::getChannelMName(channelName, m_observer);

    TaskTemplatePtr taskSwitch = TaskEmobDischarge::create(m_pcbInterface, channelMName, false, TRANSACTION_TIMEOUT);
    TaskRpcTransactionWrapperPtr taskRpcWrapper = TaskRpcTaskWrapper::create(std::move(taskSwitch), callId);
    connect(taskRpcWrapper.get(), &TaskRpcTaskWrapper::sigRpcFinished, this, &RpcDischargeOff::onRpcTaskFinish);
    m_taskQueue->addSub(std::move(taskRpcWrapper));
}

void RpcDischargeOff::onRpcTaskFinish(bool ok, QUuid rpcCallId)
{
    if(ok)
        sendRpcResult(rpcCallId, QVariant());
    else
        sendRpcError(rpcCallId, QString());
}

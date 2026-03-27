#include "rpcreadpruefgroessenstatus.h"
#include "servicechannelnamehelper.h"
#include "taskemobreadpruefgroessenstatus.h"
#include <taskcontainerqueue.h>
#include <taskrpctaskwrapper.h>
#include <vf-cpp-rpc-signature.h>

RPCReadPruefgroessenStatus::RPCReadPruefgroessenStatus(Zera::PcbInterfacePtr pcbInterface,
                                                       ChannelRangeObserver::SystemObserverPtr observer,
                                                       VeinEvent::EventSystem *eventSystem, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature (
                                  "RPC_readPruefgroessenStatus", VfCpp::VfCppRpcSignature::RPCParams({{"p_channelName", "QString"}}))),
    m_pcbInterface(pcbInterface),
    m_readErrorQueue(TaskContainerQueue::create()),
    m_emobPruefgroessenStatus(std::make_shared<int>()),
    m_observer(observer)
{
}

void RPCReadPruefgroessenStatus::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    QString channelName = parameters["p_channelName"].toString();
    QString channelMName = ServiceChannelNameHelper::getChannelMName(channelName, m_observer);

    TaskTemplatePtr taskReadState = TaskEmobReadPruefgroessenStatus::create(m_pcbInterface,
                                                                            m_emobPruefgroessenStatus,
                                                                            channelMName,
                                                                            TRANSACTION_TIMEOUT);
    TaskRpcTransactionWrapperPtr taskRpcWrapper = TaskRpcTaskWrapper::create(std::move(taskReadState), callId);
    connect(taskRpcWrapper.get(), &TaskRpcTaskWrapper::sigRpcFinished,
            this, &RPCReadPruefgroessenStatus::onRpcTaskFinish);
    m_readErrorQueue->addSub(std::move(taskRpcWrapper));
}

void RPCReadPruefgroessenStatus::onRpcTaskFinish(bool ok, QUuid rpcCallId)
{
    if(ok)
        sendRpcResult(rpcCallId, *m_emobPruefgroessenStatus);
    else
        sendRpcError(rpcCallId, QString::number(ErrorValueReturnedOnNak));
}

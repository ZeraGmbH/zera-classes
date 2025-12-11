#include "rpcreadrecordedvalues.h"
#include <vf-cpp-rpc-signature.h>

RPCReadRecordedValues::RPCReadRecordedValues(VeinEvent::EventSystem *eventSystem, std::shared_ptr<VeinDataCollector> collector, int entityId) :
    m_collector(collector),
    VfCpp::VfCppRpcSimplified(eventSystem, entityId,
                              VfCpp::VfCppRpcSignature::createRpcSignature(
                                  "RPC_ReadRecordedValues", VfCpp::VfCppRpcSignature::RPCParams({{"p_startingPoint", "int"}, {"p_endingPoint", "int"}})))
{
    connect(m_collector.get(), &VeinDataCollector::sigStoredValue, this, &RPCReadRecordedValues::onRpcFinish);
}

void RPCReadRecordedValues::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    int startingPoint = parameters["p_startingPoint"].toInt();
    int endingPoint = parameters["p_endingPoint"].toInt();
    m_collector->getStoredValues(callId, startingPoint, endingPoint);
}

void RPCReadRecordedValues::onRpcFinish(QUuid callId, bool success, QString errorMsg, QJsonObject values)
{
    if(success)
        sendRpcResult(callId, values.toVariantMap());
    else
        sendRpcError(callId, errorMsg);
}

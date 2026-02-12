#include "rpcgetrecordeddatasampler.h"
#include "recorderdatareducer.h"
#include <vf-cpp-rpc-signature.h>

using namespace VeinStorage;
static constexpr int RecorderEntityId = 1800;

RPCGetRecordedDataSampler::RPCGetRecordedDataSampler(VeinEvent::EventSystem *eventSystem, int entityId, AbstractDatabase *storage) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId, createRpcSignature()),
    m_storage(storage)
{
}

void RPCGetRecordedDataSampler::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    int samplingFactor = parameters["p_factor"].toInt();
    RecorderDataReducer dataReducer(m_storage, samplingFactor, callId);
    connect(&dataReducer, &RecorderDataReducer::sigSampledJsonCreated,
            this, &RPCGetRecordedDataSampler::sendResult);
}

QString RPCGetRecordedDataSampler::createRpcSignature()
{
    return VfCpp::VfCppRpcSignature::createRpcSignature(
        "RPC_GetRecordedDataSampler",
        VfCpp::VfCppRpcSignature::RPCParams({{"p_factor", "int"}}));
}

void RPCGetRecordedDataSampler::sendResult(QUuid callId, bool success, QString errorMsg, QJsonObject values)
{
    if(success)
        sendRpcResult(callId, values.toVariantMap());
    else
        sendRpcError(callId, errorMsg);
}

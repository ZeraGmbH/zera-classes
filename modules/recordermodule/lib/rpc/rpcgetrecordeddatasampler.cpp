#include "rpcgetrecordeddatasampler.h"
#include <vf-cpp-rpc-signature.h>

using namespace VeinStorage;
static constexpr int RecorderEntityId = 1800;

RPCGetRecordedDataSampler::RPCGetRecordedDataSampler(VeinEvent::EventSystem *eventSystem, StorageRecordDataPtr recordedData, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId, createRpcSignature()),
    m_recordedData(recordedData)
{
}

void RPCGetRecordedDataSampler::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    const RecordListStored& allRecordedData = m_recordedData->m_recordedData;
    if(allRecordedData.isEmpty())
        sendRpcError(callId, "Few values has been recorded");

    QVariantMap result;
    RecordEntityComponentSequence entityComponents = RecordParamSequencer::toSequence(m_recordedData->m_componentInfo);
    for (int i=0; i<allRecordedData.count(); ++i) {
        RecordEntryStored entry = allRecordedData[i];
        // Reorganize
        QMap<int /*entityId*/, QMap<QString /*componentName*/, float>> timeStampResultsTmp;
        for(int entityComponentNo=0; entityComponentNo<entityComponents.count(); ++entityComponentNo) {
            int entityId = entityComponents[entityComponentNo].m_entityId;
            const QString &componentName = entityComponents[entityComponentNo].m_component.m_componentName;
            float value = allRecordedData[i].m_values[entityComponentNo];
            timeStampResultsTmp[entityId][componentName] = value;
        }
        // create result
        QVariantMap timeStampResults;
        for (auto iterEntityId=timeStampResultsTmp.constBegin(); iterEntityId!=timeStampResultsTmp.constEnd(); iterEntityId++) {
            QString entityIdStr = QString("%1").arg(iterEntityId.key());
            const QMap<QString /*componentName*/, float> &entityResultsTmp = iterEntityId.value();
            QVariantMap entityResults;
            for (auto componentIter=entityResultsTmp.constBegin(); componentIter!=entityResultsTmp.constEnd(); componentIter++) {
                const QString &componentName = componentIter.key();
                float value = componentIter.value();
                entityResults[componentName] = value;
            }
            timeStampResults[entityIdStr] = entityResults;
        }
        QString timeStamp = intToStringWithLeadingDigits(entry.m_timeDiffToFirstInMs);
        result[timeStamp] = timeStampResults;
    }
    sendRpcResult(callId, result);
}

QString RPCGetRecordedDataSampler::createRpcSignature()
{
    return VfCpp::VfCppRpcSignature::createRpcSignature(
        "RPC_GetRecordedDataSampler",
        VfCpp::VfCppRpcSignature::RPCParams({}));
}

QString RPCGetRecordedDataSampler::intToStringWithLeadingDigits(int number)
{
    return QString("0000000000%1").arg(QString::number(number)).right(10);
}


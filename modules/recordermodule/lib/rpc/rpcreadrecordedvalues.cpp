#include "rpcreadrecordedvalues.h"
#include <vf-cpp-rpc-signature.h>
#include <QJsonObject>
#include <QJsonArray>

using namespace VeinStorage;

RPCReadRecordedValues::RPCReadRecordedValues(VeinEvent::EventSystem *eventSystem, VeinStorage::StorageRecordDataPtr recordedStorage, int entityId) :
    VfCpp::VfCppRpcSimplified(eventSystem, entityId, createRpcSignature()),
    m_recordedStorage(recordedStorage)
{
}

QString RPCReadRecordedValues::intToStringWithLeadingDigits(int number)
{
    return QString("0000000000%1").arg(QString::number(number)).right(10);
}

void RPCReadRecordedValues::callRPCFunction(const QUuid &callId, const QVariantMap &parameters)
{
    int startingPoint = parameters["p_startingPoint"].toInt();
    int endingPoint = parameters["p_endingPoint"].toInt();

    const RecordListStored& allRecordedData = m_recordedStorage->m_recordedData;
    if (startingPoint >= endingPoint || endingPoint > allRecordedData.count()) {
        sendRpcError(callId, "start or end not matching");
        return;
    }

    QVariantMap result;
    RecordEntityComponentSequence entityComponents = RecordParamSequencer::toSequence(m_recordedStorage->m_componentInfo);
    for (int i=startingPoint; i<endingPoint; ++i) {
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

QString RPCReadRecordedValues::createRpcSignature()
{
    return VfCpp::VfCppRpcSignature::createRpcSignature(
        "RPC_ReadRecordedValues",
        VfCpp::VfCppRpcSignature::RPCParams({{"p_startingPoint", "int"}, {"p_endingPoint", "int"}}));
}


#include "recorderdatareducer.h"
#include <vf_rpc_invoker.h>
#include <vf_client_rpc_invoker.h>
#include <vcmp_remoteproceduredata.h>

namespace VeinStorage
{
static constexpr int RecorderEntityId = 1800;

RecorderDataReducer::RecorderDataReducer(AbstractDatabase *storage, int reducerFactor, const QUuid &callId) :
    m_reducerFactor(reducerFactor),
    m_callId(callId)
{
    AbstractComponentPtr numberOfPointsComponent = storage->getFutureComponent(RecorderEntityId, "ACT_Points");
    connect(numberOfPointsComponent.get(), &AbstractComponent::sigValueChange,
             this, &RecorderDataReducer::onCountChanged);
}

void RecorderDataReducer::onCountChanged(QVariant value)
{
    for(int count=0; count<value.toInt(); count++) {
        if(count % m_reducerFactor == 0 && count != 0) {
            VfRPCInvokerPtr rpcInvoker = VfRPCInvoker::create(RecorderEntityId, std::make_unique<VfClientRPCInvoker>());
            connect(rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished, this, &RecorderDataReducer::onRpcFinish);

            QVariantMap parameters;
            parameters.insert("p_startingPoint", count - m_reducerFactor);
            parameters.insert("p_endingPoint", count);
            rpcInvoker->invokeRPC("RPC_ReadRecordedValues", parameters);
        }
    }
}

void RecorderDataReducer::onRpcFinish(bool ok, const QVariantMap &resultData)
{
    bool rpcSuccessful = (resultData[VeinComponent::RemoteProcedureData::s_resultCodeString] == VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);
    if(ok && rpcSuccessful) {
        QJsonObject result = resultData[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();

        QList<int> timeDiffList;
        ComponentValues componentValuesMap;
        EntitiesData entitiesData;

        for (auto it = result.begin(); it != result.end(); ++it) {
            timeDiffList.append(it.key().toInt());
            const QJsonObject entitiesDataJson = it.value().toObject();

            for (auto entityIt = entitiesDataJson.begin(); entityIt != entitiesDataJson.end(); ++entityIt) {
                int entityId = entityIt.key().toInt();
                const QJsonObject componentsDataJson = entityIt.value().toObject();

                for (auto iterComponents = componentsDataJson.begin(); iterComponents != componentsDataJson .end(); ++iterComponents) {
                    const QString componentName = iterComponents.key();
                    float value = iterComponents.value().toDouble();

                    if(componentValuesMap.contains(componentName)) {
                        QList<float> valuesList = componentValuesMap.value(componentName);
                        valuesList.append(value);
                        componentValuesMap[componentName] = valuesList;
                    }
                    else
                        componentValuesMap[componentName] = QList<float>() << value;
                }
                entitiesData[entityId] = componentValuesMap;
            }
        }
        int timeDiffAverage = calculateTimeDiffAverage(timeDiffList);
        EntityDataAverage valuesAverage = calculateValuesAverage(entitiesData);
        createAveragedJson(timeDiffAverage, valuesAverage);
    }
    else
        emit sigSampledJsonCreated(m_callId, false, "RPC failed", QJsonObject());
}

int RecorderDataReducer::calculateTimeDiffAverage(const QList<int> &list)
{
    int addedVals = 0;
    for(int timeDiffToFirstInMs : list)
        addedVals+= timeDiffToFirstInMs;
    return static_cast<int>(addedVals/m_reducerFactor);
}

RecorderDataReducer::EntityDataAverage RecorderDataReducer::calculateValuesAverage(const EntitiesData &entitiesValuesMap)
{
    EntityDataAverage result;
    for(auto entity: entitiesValuesMap.keys()) {
        ComponentValues values = entitiesValuesMap[entity];
        QMap<QString, float> value;
        for(auto component : values.keys()) {
            float addedVals;
            for(int value : values[component])
                addedVals+= value;
            value[component] = addedVals/m_reducerFactor;
        }
        result[entity] = value;
    }
    return result;
}

void RecorderDataReducer::createAveragedJson(int timeDiff, const EntityDataAverage &entitiesData)
{
    QJsonObject entityContainer;
    for (auto it = entitiesData.begin(); it != entitiesData.end(); ++it) {
        QJsonObject componentsObj;
        for (auto compIt = it.value().begin(); compIt != it.value().end(); ++compIt) {
            componentsObj.insert(compIt.key(), compIt.value());
        }
        entityContainer.insert(QString::number(it.key()), componentsObj);
    }
    QString timeDiffStr = QString("0000000000%1").arg(QString::number(timeDiff)).right(10);
    m_reducedJson.insert(timeDiffStr, entityContainer);
    emit sigSampledJsonCreated(m_callId, true, "", m_reducedJson);
}

}

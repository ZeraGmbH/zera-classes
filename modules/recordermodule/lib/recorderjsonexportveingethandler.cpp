#include "recorderjsonexportveingethandler.h"
#include <jsonprecisionexport.h>
#include <QJsonObject>

RecorderJsonExportVeinGetHandler::RecorderJsonExportVeinGetHandler(VeinStorage::StorageRecordDataPtr storedData) :
    m_storedData(storedData)
{
}

const QVariant &RecorderJsonExportVeinGetHandler::getCustomizedValue(const QVariant &currentValue)
{
    Q_UNUSED(currentValue)
    QJsonObject json;

    if(!m_storedData->m_recordedData.isEmpty()) {
        QDateTime localDateTime = m_storedData->m_timeOfFirstRecord;
        json.insert("timestamp_first_utc", localDateTime.toUTC().toString("yyyy-MM-dd HH:mm:ss:zzz"));
        json.insert("timestamp_first_local", localDateTime.toString("yyyy-MM-dd HH:mm:ss:zzz"));

        QJsonArray sequenceArray = getLabelsArray();
        json.insert("value_sequence", sequenceArray);

        QJsonArray jsonArray;
        int recordedValues  = m_storedData->m_recordedData.count();
        for(int i=0; i<recordedValues; i++) {
            QJsonObject recordedValuesObject;
            int timeDiffSecs = m_storedData->m_recordedData.at(i).m_timeDiffToFirstInMs;
            recordedValuesObject.insert("ts", timeDiffSecs);
            QVector<float> numbers = m_storedData->m_recordedData.at(i).m_values;
            QJsonArray values = convertVectorToJsonArray(numbers);
            recordedValuesObject.insert("data", values);

            jsonArray.append(recordedValuesObject);
        }
        json.insert("values", jsonArray);
    }

    m_jsonExport = JsonPrecisionExport::objectToString(json, 2, 7);
    return m_jsonExport;
}

QJsonArray RecorderJsonExportVeinGetHandler::getLabelsArray()
{
    QJsonArray sequenceArray;
    int recordedValues  = m_storedData->m_componentInfo.count();
    QList<int> keys = m_storedData->m_componentInfo.keys();
    for(int i=0; i<recordedValues; i++) {
        int entityId = keys.at(i);
        QList<VeinStorage::RecordComponent> components = m_storedData->m_componentInfo.value(entityId);
        for(const VeinStorage::RecordComponent &comp : components) {
            QString label = comp.m_label;
            sequenceArray.append(label);
        }
    }
    return sequenceArray;
}

QJsonArray RecorderJsonExportVeinGetHandler::convertVectorToJsonArray(QVector<float> numbers)
{
    QJsonArray jsonArray;
    for (float number : numbers) {
        jsonArray.append(number);
    }
    return jsonArray;
}

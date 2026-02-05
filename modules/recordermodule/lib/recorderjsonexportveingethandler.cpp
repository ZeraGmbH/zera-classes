#include "recorderjsonexportveingethandler.h"
#include <QJsonObject>
#include <QJsonDocument>

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
        json.insert("timestamp_first_utc", localDateTime.toUTC().toString(Qt::TextDate));
        json.insert("timestamp_first_local", localDateTime.toString(Qt::TextDate));

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

    QJsonDocument doc(json);
    m_jsonExport = doc.toJson(QJsonDocument::Indented);
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

#include "jsontimegrouping.h"
#include <QHash>
#include <QVariant>

QJsonObject JsonTimeGrouping::regroupTimestamp(QJsonObject json)
{
    QJsonObject regroupedJson;
    QMap<qint64, QJsonObject> timedMap = jsonToTimedMap(json);
    QMap<qint64, QJsonObject> groupedMap = groupTimedMap(timedMap);
    QMap<QString, QJsonObject> groupedMapDateTime;
    for(const auto &key : groupedMap.keys()) {
        QDateTime dateTime;
        dateTime.setMSecsSinceEpoch(key);
        QString strDateTime = dateTime.toString("dd-MM-yyyy hh:mm:ss.zzz");
        groupedMapDateTime[strDateTime] = groupedMap[key];
    }
    regroupedJson = convertMapToJsonObject(groupedMapDateTime);
    return regroupedJson;
}

QJsonObject JsonTimeGrouping::convertMapToJsonObject(const QMap<QString, QJsonObject> map)
{
    QJsonObject jsonObject;
    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        jsonObject.insert(it.key(), it.value());
    }
    return jsonObject;
}

QMap<qint64, QJsonObject> JsonTimeGrouping::jsonToTimedMap(const QJsonObject &json)
{
    QMap<qint64, QJsonObject> dateTimeJsonMap;
    const QStringList keys = json.keys();
    for(const QString &key : keys) {
        qint64 dateTimeMs = QDateTime::fromString(key, "dd-MM-yyyy hh:mm:ss.zzz").toMSecsSinceEpoch();
        dateTimeJsonMap[dateTimeMs] = json[key].toObject();
    }
    return dateTimeJsonMap;
}

const qint64 maxTimeDiffMs = 20;

QMap<qint64, QJsonObject> JsonTimeGrouping::groupTimedMap(const QMap<qint64, QJsonObject> timedMap)
{
    QMap<qint64, QJsonObject> ret;
    qint64 currTimeStampStored = 0;
    for(auto iter = timedMap.cbegin(); iter != timedMap.cend(); ++iter) {
        qint64 entryDateTime = iter.key();
        bool append = false;
        if(!ret.contains(entryDateTime)) {
            if(entryDateTime-currTimeStampStored > maxTimeDiffMs) {
                currTimeStampStored = entryDateTime;
                ret[currTimeStampStored] = iter.value();
            }
            else
                append = true;
        }
        if(append) {
            QJsonObject jsoncurrTimeStamp = ret[currTimeStampStored];
            QJsonObject lastValueArray = iter.value();
            appendValuesToJson(jsoncurrTimeStamp, lastValueArray);
            ret[currTimeStampStored] = jsoncurrTimeStamp;
        }
    }
    return ret;
}

void JsonTimeGrouping::appendValuesToJson(QJsonObject& mergedJson, QJsonObject objWithoutTime)
{
    for(const auto entityKey : objWithoutTime.keys()) {
        if(mergedJson.contains(entityKey)) {
            QJsonValue existingValue = mergedJson.value(entityKey);
            QHash<QString, QVariant> hash= existingValue.toObject().toVariantHash();
            QHash<QString, QVariant> valuesToAppend = objWithoutTime.value(entityKey).toObject().toVariantHash();
            for(auto hashIt = valuesToAppend.constBegin(); hashIt != valuesToAppend.constEnd(); ++hashIt)
                hash.insert(hashIt.key(), hashIt.value());
            mergedJson.insert(entityKey, QJsonObject::fromVariantHash(hash));
        }
        else
            mergedJson.insert(entityKey, objWithoutTime.value(entityKey));
    }
}

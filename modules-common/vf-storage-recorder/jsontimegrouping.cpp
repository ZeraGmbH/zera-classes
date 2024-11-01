#include "jsontimegrouping.h"
#include <QVariant>
#include <QDateTime>

QJsonObject JsonTimeGrouping::regroupTimestamp(QJsonObject json)
{
    QMap<qint64, QJsonObject> timedMap = jsonToMsSinceEpochValuesMap(json);
    QMap<qint64, QJsonObject> groupedMap = groupToMsSinceEpochValuesMap(timedMap);
    return msSinceEpochValueMapToJson(groupedMap);
}

static const char* JsonDateTimeFormat = "dd-MM-yyyy hh:mm:ss.zzz";
static const qint64 maxTimeDiffMs = 100;

QMap<qint64, QJsonObject> JsonTimeGrouping::jsonToMsSinceEpochValuesMap(const QJsonObject &json)
{
    QMap<qint64, QJsonObject> dateTimeJsonMap;
    const QStringList keys = json.keys();
    for(const QString &key : keys) {
        qint64 dateTimeMs = QDateTime::fromString(key, JsonDateTimeFormat).toMSecsSinceEpoch();
        dateTimeJsonMap[dateTimeMs] = json[key].toObject();
    }
    return dateTimeJsonMap;
}

QMap<qint64, QJsonObject> JsonTimeGrouping::groupToMsSinceEpochValuesMap(const QMap<qint64, QJsonObject> timedMap)
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

QJsonObject JsonTimeGrouping::msSinceEpochValueMapToJson(const QMap<qint64, QJsonObject> map)
{
    QJsonObject jsonObject;
    for(auto it = map.constBegin(); it != map.constEnd(); ++it) {
        QString strDateTime = QDateTime::fromMSecsSinceEpoch(it.key()).toString(JsonDateTimeFormat);
        jsonObject.insert(strDateTime, it.value());
    }
    return jsonObject;
}

void JsonTimeGrouping::appendValuesToJson(QJsonObject& mergedJson, QJsonObject objWithoutTime)
{
    const QStringList keys = objWithoutTime.keys();
    for(const auto &entityKey : keys) {
        if(mergedJson.contains(entityKey)) {
            QJsonValue existingValue = mergedJson.value(entityKey);
            QHash<QString, QVariant> hash = existingValue.toObject().toVariantHash();
            QHash<QString, QVariant> valuesToAppend = objWithoutTime.value(entityKey).toObject().toVariantHash();
            for(auto hashIt = valuesToAppend.constBegin(); hashIt != valuesToAppend.constEnd(); ++hashIt)
                hash.insert(hashIt.key(), hashIt.value());
            mergedJson.insert(entityKey, QJsonObject::fromVariantHash(hash));
        }
        else
            mergedJson.insert(entityKey, objWithoutTime.value(entityKey));
    }
}

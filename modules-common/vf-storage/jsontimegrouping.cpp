#include "jsontimegrouping.h"
#include <QSet>

JsonTimeGrouping::JsonTimeGrouping(QJsonObject json) :
    m_json(json)
{
}

QJsonObject JsonTimeGrouping::regroupTimestamp()
{
    QJsonObject finalJson;

    QMap<qint64, QJsonObject> timedMap = jsonToTimedMap(m_json);
    QMap<qint64, QJsonObject> groupedMap = groupTimedMap(timedMap);
    QMap<QString, QJsonObject> groupedMapDateTime;
    for(const auto &key : groupedMap.keys()) {
        QDateTime dateTime;
        dateTime.setMSecsSinceEpoch(key);
        QString strDateTime = dateTime.toString("dd-MM-yyyy hh:mm:ss.zzz");
        groupedMapDateTime[strDateTime] = groupedMap[key];
    }
    finalJson = convertMapToJsonObject(groupedMapDateTime);

    QJsonObject regroupedJson;
    QHash<int, QList<QDateTime>> approxTimestamps = getApproximativeTimestamps();
    for (auto it = approxTimestamps.constBegin(); it != approxTimestamps.constEnd(); ++it) {
        QJsonObject mergedJson;
        QList<QDateTime> Timelist = it.value();
        for(int i = 0; i < Timelist.size(); i++) {
            QString strTime = Timelist[i].toString("dd-MM-yyyy hh:mm:ss.zzz");
            QJsonValue value = m_json.value(strTime);
            QJsonObject objWithoutTime = value.toObject();
            appendValuesToJson(mergedJson, objWithoutTime);
        }
        QString lastTimestamp = Timelist.last().toString("dd-MM-yyyy hh:mm:ss.zzz");
        regroupedJson.insert(lastTimestamp, mergedJson);
    }
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
        /*else
            append = true;*/
        if(append) {
            QJsonObject jsoncurrTimeStamp = ret[currTimeStampStored];
            QJsonObject lastValueArray = iter.value();
            appendValuesToJson(jsoncurrTimeStamp, lastValueArray);
            ret[currTimeStampStored] = jsoncurrTimeStamp;
        }
    }
    return ret;
}

QList<QDateTime> JsonTimeGrouping::getTimeInJson()
{
    QList<QDateTime> timeList;
    for(const QString &key : m_json.keys()) {
        QDateTime dateTime = QDateTime::fromString(key, "dd-MM-yyyy hh:mm:ss.zzz");
        timeList.append(dateTime);
    }
    return timeList;
}

QHash<int, QList<QDateTime>> JsonTimeGrouping::getApproximativeTimestamps()
{
    QHash<int, QList<QDateTime>> approxTimestamps;
    QList<QDateTime> timeList = getTimeInJson();
    QSet<QDateTime> dateTimeSet;
    int hashKey = 0;
    for(int i = 0; i < timeList.size(); i++) {
        if(timeList[i] == timeList.last()) {
            if(approxTimestamps.contains(hashKey)) {
                if(!approxTimestamps[hashKey].contains(timeList[i]))
                    hashKey ++;
                else
                    break;
            }
            appendOneTimestamp(&approxTimestamps, hashKey, timeList[i]);
        }
        else {
            int ms = timeList[i].msecsTo(timeList[i+1]);
            if(std::abs(ms) <= 20) {
                dateTimeSet.insert(timeList[i]);
                dateTimeSet.insert(timeList[i+1]);
                approxTimestamps[hashKey] = QList<QDateTime> (dateTimeSet.begin(), dateTimeSet.end());
            }
            else {
                dateTimeSet.clear();
                if(approxTimestamps.contains(hashKey)) {
                    if(approxTimestamps[hashKey].contains(timeList[i])) {
                        hashKey++;
                        continue;
                    }
                }
                appendOneTimestamp(&approxTimestamps, hashKey, timeList[i]);
                hashKey++;
            }
        }
    }
    return approxTimestamps;
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

void JsonTimeGrouping::appendOneTimestamp(QHash<int, QList<QDateTime>> *approxTimestamps, int hashKey, QDateTime timestamp)
{
    QList<QDateTime> oneElt;
    oneElt.append(timestamp);
    approxTimestamps->insert(hashKey, oneElt);
}

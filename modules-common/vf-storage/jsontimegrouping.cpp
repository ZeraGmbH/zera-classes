#include <QSet>
#include "jsontimegrouping.h"

JsonTimeGrouping::JsonTimeGrouping(QJsonObject json) :
    m_json(json)
{
}

QJsonObject JsonTimeGrouping::regroupTimestamp()
{
    QJsonObject mergedJson;
    QHash<int, QList<QDateTime>> approxTimestamps = getApproximativeTimestamps();
    for (auto it = approxTimestamps.constBegin(); it != approxTimestamps.constEnd(); ++it) {
        clearJson();
        QList<QDateTime> Timelist = it.value();
        for(int i = 0; i < Timelist.size(); i++) {
            QString strTime = Timelist[i].toString("dd-MM-yyyy hh:mm:ss.zzz");
            QJsonValue value = m_json.value(strTime);
            QJsonObject objWithoutTime = value.toObject();
            appendValuesToJson(objWithoutTime);
        }
        QString lastTimestamp = Timelist.last().toString("dd-MM-yyyy hh:mm:ss.zzz");
        mergedJson.insert(lastTimestamp, m_mergedJson);
    }
    return mergedJson;
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

void JsonTimeGrouping::appendValuesToJson(QJsonObject objWithoutTime)
{
    for(const auto entityKey : objWithoutTime.keys()) {
        if(m_mergedJson.contains(entityKey)) {
            QJsonValue existingValue = m_mergedJson.value(entityKey);
            QHash<QString, QVariant> hash= existingValue.toObject().toVariantHash();
            QHash<QString, QVariant> valuesToAppend = objWithoutTime.value(entityKey).toObject().toVariantHash();
            for(auto hashIt = valuesToAppend.constBegin(); hashIt != valuesToAppend.constEnd(); ++hashIt)
                hash.insert(hashIt.key(), hashIt.value());
            m_mergedJson.insert(entityKey, QJsonObject::fromVariantHash(hash));
        }
        else
            m_mergedJson.insert(entityKey, objWithoutTime.value(entityKey));
    }
}

void JsonTimeGrouping::appendOneTimestamp(QHash<int, QList<QDateTime>> *approxTimestamps, int hashKey, QDateTime timestamp)
{
    QList<QDateTime> oneElt;
    oneElt.append(timestamp);
    approxTimestamps->insert(hashKey, oneElt);
}

void JsonTimeGrouping::clearJson()
{
    if(!m_mergedJson.isEmpty()) {
        for (const QString &key : m_mergedJson.keys()) {
            m_mergedJson.remove(key);
        }
    }
}


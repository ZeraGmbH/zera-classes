#ifndef JSONTIMEGROUPING_H
#define JSONTIMEGROUPING_H

#include "qelapsedtimer.h"
#include <QJsonObject>
#include <QMap>

class JsonTimeGrouping
{
public:
    static QJsonObject regroupTimestamp(QJsonObject json);

private:
    static QMap<qint64, QJsonObject> jsonToMsSinceEpochValuesMap(const QJsonObject &json);
    static QMap<qint64, QJsonObject> groupToMsSinceEpochValuesMap(const QMap<qint64, QJsonObject> timedMap);
    static QJsonObject msSinceEpochValueMapToJson(const QMap<qint64, QJsonObject> map);
    static void appendValuesToJson(QJsonObject &mergedJson, QJsonObject objWithoutTime);

    static QElapsedTimer m_regroupingTime;
};

#endif // JSONTIMEGROUPING_H

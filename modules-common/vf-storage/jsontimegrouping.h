#ifndef JSONTIMEGROUPING_H
#define JSONTIMEGROUPING_H

#include <QDateTime>
#include <QJsonObject>
#include <QMap>
#include <QObject>

class JsonTimeGrouping : public QObject
{
    Q_OBJECT
public:
    static QJsonObject regroupTimestamp(QJsonObject json);

private:
    static QMap<qint64, QJsonObject> jsonToTimedMap(const QJsonObject &json);
    static QMap<qint64, QJsonObject> groupTimedMap(const QMap<qint64, QJsonObject> timedMap);
    static QJsonObject convertMapToJsonObject(const QMap<qint64, QJsonObject> map);
    static void appendValuesToJson(QJsonObject &mergedJson, QJsonObject objWithoutTime);
};

#endif // JSONTIMEGROUPING_H

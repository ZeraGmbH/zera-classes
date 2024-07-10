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
    JsonTimeGrouping(QJsonObject json);
    QJsonObject regroupTimestamp();

private:
    QMap<qint64, QJsonObject> jsonToTimedMap(const QJsonObject &json);
    QMap<qint64, QJsonObject> groupTimedMap(const QMap<qint64, QJsonObject> timedMap);
    QJsonObject convertMapToJsonObject(const QMap<QString, QJsonObject> map);
    void appendValuesToJson(QJsonObject &mergedJson, QJsonObject objWithoutTime);

    QJsonObject m_json;
};

#endif // JSONTIMEGROUPING_H

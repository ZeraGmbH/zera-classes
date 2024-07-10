#ifndef JSONTIMEGROUPING_H
#define JSONTIMEGROUPING_H

#include <QDateTime>
#include <QJsonObject>
#include <QHash>
#include <QObject>

class JsonTimeGrouping : public QObject
{
    Q_OBJECT
public:
    JsonTimeGrouping(QJsonObject json);
    QJsonObject regroupTimestamp();

private:
    QList<QDateTime> getTimeInJson();
    QHash<int, QList<QDateTime>> getApproximativeTimestamps();
    void appendValuesToJson(QJsonObject &mergedJson, QJsonObject objWithoutTime);
    void appendOneTimestamp(QHash<int, QList<QDateTime>> *approxTimestamps, int hashKey, QDateTime timestamp);

    QJsonObject m_json;
};

#endif // JSONTIMEGROUPING_H

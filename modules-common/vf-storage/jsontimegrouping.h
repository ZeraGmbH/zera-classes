#ifndef JSONTIMEGROUPING_H
#define JSONTIMEGROUPING_H

#include <QDateTime>
#include <QJsonObject>
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
    void appendValuesToJson(QJsonObject objWithoutTime);
    void appendOneTimestamp(QHash<int, QList<QDateTime>> *approxTimestamps, int hashKey, QDateTime timestamp);
    void clearJson();

    QJsonObject m_json;
    QJsonObject m_mergedJson;
};

#endif // JSONTIMEGROUPING_H

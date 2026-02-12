#ifndef RECORDERDATAREDUCER_H
#define RECORDERDATAREDUCER_H

#include <vs_abstractdatabase.h>
#include <QJsonObject>
#include <QObject>

namespace VeinStorage {

class RecorderDataReducer : public QObject
{
    typedef QMap<QString /*componentname*/, QList<float> /*valuesList*/> ComponentValues;
    typedef QMap<int /*entityId*/, ComponentValues> EntitiesData;
    typedef QMap<int /*entityId*/, QMap<QString /*componentname*/, float /*value*/>> EntityDataAverage;

    Q_OBJECT
public:
    explicit RecorderDataReducer(AbstractDatabase *storage, int reducerFactor, const QUuid &callId);

signals:
    void sigSampledJsonCreated(QUuid callId, bool success, QString errorMsg, QJsonObject values);

private slots:
    void onCountChanged(QVariant value);
    void onRpcFinish(bool ok, const QVariantMap &resultData);

private:
    int calculateTimeDiffAverage(const QList<int> &list);
    EntityDataAverage calculateValuesAverage(const EntitiesData &entitiesValuesMap);
    void createAveragedJson(int timeDiff, const EntityDataAverage &entitiesData);

    int m_reducerFactor;
    QUuid m_callId;
    QJsonObject m_reducedJson;
};

}
#endif // RECORDERDATAREDUCER_H

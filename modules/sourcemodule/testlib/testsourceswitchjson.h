#ifndef TESTSOURCESWITCHJSON_H
#define TESTSOURCESWITCHJSON_H

#include <abstractsourceswitchjson.h>
#include <QQueue>

class TestSourceSwitchJson : public AbstractSourceSwitchJson
{
    Q_OBJECT
public:
    static AbstractSourceSwitchJsonPtr create(const QJsonObject &sourceCapabilities);
    explicit TestSourceSwitchJson(const QJsonObject &sourceCapabilities);

    int switchState(const JsonParamApi &paramState) override;
    JsonParamApi getCurrLoadpoint() override;

    void fireFinish(bool ok);
private:
    JsonParamApi m_loadpointCurrent;

    int m_currentId = 0;
    struct QueueEntry {
        int switchId = 0;
        JsonParamApi loadpoint;
    };
    QQueue<QueueEntry> m_pendingLoadpoints;
};

#endif // TESTSOURCESWITCHJSON_H

#ifndef TESTSOURCESWITCHJSON_H
#define TESTSOURCESWITCHJSON_H

#include <abstractsourceswitchjson.h>
#include <QQueue>

class TestSourceSwitchJson : public AbstractSourceSwitchJson
{
    Q_OBJECT
public:
    static std::shared_ptr<TestSourceSwitchJson> create(const QJsonObject &jsonStruct);
    explicit TestSourceSwitchJson(const QJsonObject &jsonStruct);

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

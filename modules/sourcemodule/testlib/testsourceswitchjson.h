#ifndef TESTSOURCESWITCHJSON_H
#define TESTSOURCESWITCHJSON_H

#include <abstractsourceswitchjson.h>
#include <QQueue>

class TestSourceSwitchJson : public AbstractSourceSwitchJson
{
    Q_OBJECT
public:
    int switchState(const JsonParamApi &paramState) override;
    JsonParamApi getCurrLoadpoint() override;

    void fireFinish(bool ok);
private:
    JsonParamApi m_loadpointCurrent;

    int m_currentId = 0;
    struct QueueEntry {
        int switchId;
        JsonParamApi loadpoint;
    };
    QQueue<QueueEntry> m_pendingLoadpoints;
};

#endif // TESTSOURCESWITCHJSON_H

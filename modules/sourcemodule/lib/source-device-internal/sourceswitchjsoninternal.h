#ifndef SOURCESWITCHJSONINTERNAL_H
#define SOURCESWITCHJSONINTERNAL_H

#include "persistentjsonstate.h"
#include <abstractsourceswitchjson.h>
#include <jsonstructapi.h>
#include <abstractserverInterface.h>
#include <taskcontainerinterface.h>
#include <QQueue>

class SourceSwitchJsonInternal : public AbstractSourceSwitchJson
{
    Q_OBJECT
public:
    SourceSwitchJsonInternal(AbstractServerInterfacePtr serverInterface,
                             const QJsonObject &sourceCapabilities);
    int switchState(const JsonParamApi &desiredLoad) override;
    JsonParamApi getCurrLoadpoint() override;

private slots:
    void onSwitchTasksFinish(bool ok, int taskId);
private:
    TaskTemplatePtr getSwitchOnTask(const JsonParamApi &desiredLoad);
    TaskTemplatePtr getSwitchOffTask(const JsonParamApi &desiredLoad);
    TaskContainerInterfacePtr createLoadpointTasks(const JsonParamApi &paramState);
    TaskTemplatePtr createSourceModeOnTask(const JsonParamApi &paramState);
    TaskTemplatePtr createSourceOnOffTask(const JsonParamApi &paramState);
    void doStartTask(TaskTemplatePtr task, const JsonParamApi &desiredLoad);

    AbstractServerInterfacePtr m_serverInterface;
    JsonStructApi m_sourceCapabilities;
    PersistentJsonState m_persistentParamState;
    JsonParamApi m_loadpointCurrent;
    JsonParamApi m_loadpointRequestedLast;

    TaskTemplatePtr m_pendingTask;
    struct PendingSwitchEntries
    {
        JsonParamApi desiredLoad;
        TaskTemplatePtr task;
    };
    QQueue<std::shared_ptr<PendingSwitchEntries>> m_pendingSwitchRequests;
};

#endif // SOURCESWITCHJSONINTERNAL_H

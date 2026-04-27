#ifndef SOURCESWITCHJSONINTERNAL_H
#define SOURCESWITCHJSONINTERNAL_H

#include "persistentjsonstate.h"
#include <abstractsourceswitchjson.h>
#include <jsonstructapi.h>
#include <abstractserverInterface.h>
#include <taskcontainerinterface.h>

class SourceSwitchJsonInternal : public AbstractSourceSwitchJson
{
    Q_OBJECT
public:
    SourceSwitchJsonInternal(AbstractServerInterfacePtr serverInterface,
                             const QJsonObject &sourceCapabilities);
    void switchState(const JsonParamApi &desiredLoad) override;
    JsonParamApi getCurrLoadState() override;
    JsonParamApi getRequestedLoadState() override;
signals:
    void sigSwitchTransactionStarted();

private slots:
    void onSwitchTasksFinish(bool ok);
private:
    void switchOff();
    TaskContainerInterfacePtr createLoadpointTasks(const JsonParamApi &paramState);
    TaskTemplatePtr createSourceModeOnTask(const JsonParamApi &paramState);
    TaskTemplatePtr createSourceOnOffTask(const JsonParamApi &paramState);

    AbstractServerInterfacePtr m_serverInterface;
    JsonStructApi m_sourceCapabilities;
    PersistentJsonState m_persistentParamState;
    JsonParamApi m_paramsCurrent;
    JsonParamApi m_paramsRequested;
    TaskContainerInterfacePtr m_currentTasks;
};

#endif // SOURCESWITCHJSONINTERNAL_H

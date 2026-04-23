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
    void switchState(const JsonParamApi &paramState) override;
    void switchOff() override;
    JsonParamApi getCurrLoadState() override;
    JsonParamApi getRequestedLoadState() override;

private slots:
    void onSwitchTasksFinish(bool ok);
private:
    TaskContainerInterfacePtr createAmplitudeTasks(JsonParamApi paramState);
    TaskTemplatePtr createSourceModeOnTask(const JsonParamApi &paramState);
    TaskTemplatePtr createSourceOnOffTask(const JsonParamApi &paramState);
    static QStringList getChannelMNamesSwitchedOnCommaSeparated(const JsonStructApi &sourceCapabilities,
                                                                const JsonParamApi &wantedLoadpoint);
    static QString getChannelMName(phaseType type, int phaseNoBase0);
    static QString getAlias(phaseType type, int phaseNoBase0);

    AbstractServerInterfacePtr m_serverInterface;
    JsonStructApi m_sourceCapabilities;
    PersistentJsonState m_persistentParamState;
    JsonParamApi m_paramsCurrent;
    JsonParamApi m_paramsRequested;
    TaskContainerInterfacePtr m_currentTasks;
};

#endif // SOURCESWITCHJSONINTERNAL_H

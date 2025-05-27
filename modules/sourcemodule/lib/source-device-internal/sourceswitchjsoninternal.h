#ifndef SOURCESWITCHJSONINTERNAL_H
#define SOURCESWITCHJSONINTERNAL_H

#include "persistentjsonstate.h"
#include <abstractsourceswitchjson.h>

class SourceSwitchJsonInternal : public AbstractSourceSwitchJson
{
    Q_OBJECT
public:
    SourceSwitchJsonInternal(const QJsonObject &sourceJsonStruct);
    void switchState(JsonParamApi paramState) override;
    void switchOff() override;
    JsonParamApi getCurrLoadState() override;
    JsonParamApi getRequestedLoadState() override;

private:
    const QJsonObject m_sourceJsonStruct;
    PersistentJsonState m_persistentParamState;
    JsonParamApi m_paramsCurrent;
    JsonParamApi m_paramsRequested;
};

#endif // SOURCESWITCHJSONINTERNAL_H

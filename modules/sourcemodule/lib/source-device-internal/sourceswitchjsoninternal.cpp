#include "sourceswitchjsoninternal.h"

SourceSwitchJsonInternal::SourceSwitchJsonInternal(const QJsonObject &sourceJsonStruct) :
    m_sourceJsonStruct(sourceJsonStruct),
    m_persistentParamState(PersistentJsonState(sourceJsonStruct)),
    m_paramsCurrent(m_persistentParamState.loadJsonState()),
    m_paramsRequested(m_paramsCurrent)
{
}

void SourceSwitchJsonInternal::switchState(JsonParamApi paramState)
{

}

void SourceSwitchJsonInternal::switchOff()
{

}

JsonParamApi SourceSwitchJsonInternal::getCurrLoadState()
{
    return m_paramsCurrent;
}

JsonParamApi SourceSwitchJsonInternal::getRequestedLoadState()
{
    return m_paramsRequested;
}

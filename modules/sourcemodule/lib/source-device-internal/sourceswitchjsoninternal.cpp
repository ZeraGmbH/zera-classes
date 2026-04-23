#include "sourceswitchjsoninternal.h"
#include "taskchangerangebyamplitude.h"
#include "tasksetdspamplitude.h"
#include <math.h>
#include <taskcontainerinterface.h>
#include <taskcontainersequence.h>
#include <taskcontainerparallel.h>

SourceSwitchJsonInternal::SourceSwitchJsonInternal(AbstractServerInterfacePtr serverInterface,
                                                   const QJsonObject &sourceCapabilities) :
    m_serverInterface(serverInterface),
    m_sourceCapabilitiesApi(sourceCapabilities),
    m_persistentParamState(PersistentJsonState(sourceCapabilities)),
    m_paramsCurrent(m_persistentParamState.loadJsonState()),
    m_paramsRequested(m_paramsCurrent)
{
}

TaskContainerInterfacePtr SourceSwitchJsonInternal::createAmplitudeTasks(JsonParamApi paramState)
{
    TaskContainerInterfacePtr ampTasks = TaskContainerParallel::create();
    for (phaseType type : {phaseType::U, phaseType::I}) {
        const int phaseCount = type==phaseType::U ?
                                   m_sourceCapabilitiesApi.getCountUPhases() :
                                   m_sourceCapabilitiesApi.getCountIPhases();
        for (int phaseNo=0; phaseNo < phaseCount; ++phaseNo) {
            double peakValue = paramState.getRms(type, phaseNo) * M_SQRT2;
            TaskContainerInterfacePtr rmsTasks = TaskContainerSequence::create(TaskContainerSequence::StopOnFirstTaskFail);
            rmsTasks->addSub(TaskChangeRangeByAmplitude::create(
                m_serverInterface,
                getChannelMName(type, phaseNo),
                peakValue,
                TRANSACTION_TIMEOUT,
                [=](){
                    qWarning("TaskChangeRangeByAmplitude failed for %s", qPrintable(getAlias(type, phaseNo)));
                }));
            rmsTasks->addSub(TaskSetDspAmplitude::create(
                m_serverInterface,
                getChannelMName(type, phaseNo),
                peakValue,
                TRANSACTION_TIMEOUT,
                [=](){
                    qWarning("TaskSetDspAmplitude failed for %s", qPrintable(getAlias(type, phaseNo)));
                }));
            ampTasks->addSub(std::move(rmsTasks));
        }
    }
    return ampTasks;
}

void SourceSwitchJsonInternal::switchState(JsonParamApi paramState)
{
    m_paramsRequested = paramState;
    m_currentTasks = TaskContainerSequence::create(TaskContainerSequence::StopOnFirstTaskFail);
    m_currentTasks->addSub(createAmplitudeTasks(paramState));
    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &SourceSwitchJsonInternal::onSwitchTasksFinish);
    m_currentTasks->start();
}

void SourceSwitchJsonInternal::switchOff()
{
    JsonParamApi paramOff = m_paramsCurrent;
    paramOff.setOn(false);
    switchState(paramOff);
}

JsonParamApi SourceSwitchJsonInternal::getCurrLoadState()
{
    return m_paramsCurrent;
}

JsonParamApi SourceSwitchJsonInternal::getRequestedLoadState()
{
    return m_paramsRequested;
}

void SourceSwitchJsonInternal::onSwitchTasksFinish(bool ok)
{
    m_paramsCurrent = m_paramsRequested;
    emit sigSwitchFinished();
}

QString SourceSwitchJsonInternal::getChannelMName(phaseType type, int phaseNoBase0)
{
    switch (phaseNoBase0) {
    case 0:
        return type == phaseType::U ? "m0" : "m3";
    case 1:
        return type == phaseType::U ? "m1" : "m4";
    case 2:
        return type == phaseType::U ? "m2" : "m5";
    }
    qCritical("getChannelMName: invalid parameters!");
    return "m0";
}

QString SourceSwitchJsonInternal::getAlias(phaseType type, int phaseNoBase0)
{
    switch (phaseNoBase0) {
    case 0:
        return type == phaseType::U ? "UL1" : "IL1";
    case 1:
        return type == phaseType::U ? "UL2" : "IL2";
    case 2:
        return type == phaseType::U ? "UL3" : "IL3";
    }
    qCritical("getChannelMName: invalid parameters!");
    return "m0";
}

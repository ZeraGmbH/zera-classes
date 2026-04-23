#include "sourceswitchjsoninternal.h"
#include "taskchangerangebyamplitude.h"
#include "taskgeneratormultiplephasessourcemodeon.h"
#include "taskgeneratormultiplephasesswitchon.h"
#include "tasksetdspamplitude.h"
#include "tasksetdspfrequency.h"
#include <math.h>
#include <taskcontainerinterface.h>
#include <taskcontainersequence.h>
#include <taskcontainerparallel.h>

SourceSwitchJsonInternal::SourceSwitchJsonInternal(AbstractServerInterfacePtr serverInterface,
                                                   const QJsonObject &sourceCapabilities) :
    m_serverInterface(serverInterface),
    m_sourceCapabilities(sourceCapabilities),
    m_persistentParamState(PersistentJsonState(sourceCapabilities)),
    m_paramsCurrent(m_persistentParamState.loadJsonState()),
    m_paramsRequested(m_paramsCurrent)
{
}

void SourceSwitchJsonInternal::switchState(const JsonParamApi &paramState)
{
    m_paramsRequested = paramState;
    m_currentTasks = TaskContainerSequence::create(TaskContainerSequence::StopOnFirstTaskFail);

    m_currentTasks->addSub(createSourceModeOnTask(paramState));
    m_currentTasks->addSub(createLoadpointTasks(paramState));
    m_currentTasks->addSub(createSourceOnOffTask(paramState));

    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &SourceSwitchJsonInternal::onSwitchTasksFinish);
    m_currentTasks->start();
}

void SourceSwitchJsonInternal::switchOff()
{
    JsonParamApi paramOff = m_paramsCurrent;
    paramOff.setOn(false);

    m_currentTasks = TaskContainerSequence::create(TaskContainerSequence::StopOnFirstTaskFail);

    m_currentTasks->addSub(createSourceOnOffTask(paramOff));
    m_currentTasks->addSub(createSourceModeOnTask(paramOff));
    connect(m_currentTasks.get(), &TaskTemplate::sigFinish,
            this, &SourceSwitchJsonInternal::onSwitchTasksFinish);
    m_currentTasks->start();
}

TaskContainerInterfacePtr SourceSwitchJsonInternal::createLoadpointTasks(const JsonParamApi &paramState)
{
    TaskContainerInterfacePtr parallelPhaseTasks = TaskContainerParallel::create();
    for (phaseType type : {phaseType::U, phaseType::I}) {
        const int phaseCount = type==phaseType::U ?
                                   m_sourceCapabilities.getCountUPhases() :
                                   m_sourceCapabilities.getCountIPhases();
        for (int phaseNo=0; phaseNo < phaseCount; ++phaseNo) {
            TaskContainerInterfacePtr phaseTasks = TaskContainerSequence::create(TaskContainerSequence::StopOnFirstTaskFail);
            // notes on frequency:
            // * mains sync is not implemented yet therefore default to 50Hz for now
            // * is frequency per phase a wanted and supported feature?
            double frequency = paramState.getFreqVal();
            if (frequency == 0.0)
                frequency = 50.0;
            phaseTasks->addSub(TaskSetDspFrequency::create(
                m_serverInterface,
                getChannelMName(type, phaseNo),
                frequency,
                [=]() {
                    qWarning("TaskSetDspFrequency failed for %s", qPrintable(getAlias(type, phaseNo)));
                }));

            const double peakValue = paramState.getRms(type, phaseNo) * M_SQRT2;
            phaseTasks->addSub(TaskChangeRangeByAmplitude::create(
                m_serverInterface,
                getChannelMName(type, phaseNo),
                peakValue,
                [=]() {
                    qWarning("TaskChangeRangeByAmplitude failed for %s", qPrintable(getAlias(type, phaseNo)));
                }));
            phaseTasks->addSub(TaskSetDspAmplitude::create(
                m_serverInterface,
                getChannelMName(type, phaseNo),
                peakValue,
                [=]() {
                    qWarning("TaskSetDspAmplitude failed for %s", qPrintable(getAlias(type, phaseNo)));
                }));

            parallelPhaseTasks->addSub(std::move(phaseTasks));
        }
    }
    return parallelPhaseTasks;
}

TaskTemplatePtr SourceSwitchJsonInternal::createSourceModeOnTask(const JsonParamApi &paramState)
{
    QStringList channelMNamesOn = getChannelMNamesSwitchedOnCommaSeparated(m_sourceCapabilities, paramState);
    return TaskGeneratorMultiplePhasesSourceModeOn::create(
        m_serverInterface,
        channelMNamesOn,
        [=]() { qWarning("TaskChangeRangeByAmplitude failed for %s", qPrintable(channelMNamesOn.join(","))); }
        );
}

TaskTemplatePtr SourceSwitchJsonInternal::createSourceOnOffTask(const JsonParamApi &paramState)
{
    QStringList channelMNamesOn = getChannelMNamesSwitchedOnCommaSeparated(m_sourceCapabilities, paramState);
    return TaskGeneratorMultiplePhasesSwitchOn::create(
        m_serverInterface,
        channelMNamesOn,
        [=]() { qWarning("TaskChangeRangeByAmplitude failed for %s", qPrintable(channelMNamesOn.join(","))); }
        );
}

QStringList SourceSwitchJsonInternal::getChannelMNamesSwitchedOnCommaSeparated(const JsonStructApi &sourceCapabilities,
                                                                               const JsonParamApi &wantedLoadpoint)
{
    QStringList activeModeOnChannelMNames;
    for (phaseType type : {phaseType::U, phaseType::I}) {
        const int phaseCount = type==phaseType::U ?
                                   sourceCapabilities.getCountUPhases() :
                                   sourceCapabilities.getCountIPhases();
        for (int phaseNo=0; phaseNo < phaseCount; ++phaseNo) {
            if(wantedLoadpoint.getOn() && wantedLoadpoint.getOn(type, phaseNo))
                activeModeOnChannelMNames.append(getChannelMName(type, phaseNo));
        }
    }
    return activeModeOnChannelMNames;
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


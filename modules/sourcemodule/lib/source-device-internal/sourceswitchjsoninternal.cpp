#include "sourceswitchjsoninternal.h"
#include "sourcechannelhelper.h"
#include "taskgeneratorrangebyamplitudeset.h"
#include "taskgeneratormultiplephasessourcemodeonset.h"
#include "taskgeneratormultiplephasesswitchonset.h"
#include "taskgeneratordspamplitudeset.h"
#include "taskgeneratordspangleset.h"
#include "taskgeneratordspfrequencyset.h"
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
    m_paramsRequested = paramOff;

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
        const int phaseCount = type==phaseType::U ? m_sourceCapabilities.getCountUPhases() : m_sourceCapabilities.getCountIPhases();
        for (int phaseNo=0; phaseNo < phaseCount; ++phaseNo) {
            if (!paramState.getOn(type, phaseNo))
                continue;
            TaskContainerInterfacePtr phaseTasks = TaskContainerSequence::create(TaskContainerSequence::StopOnFirstTaskFail);
            const QString phaseAlias = SourceChannelHelper::getAlias(type, phaseNo);
            // notes on frequency:
            // * mains sync is not implemented yet therefore default to 50Hz for now
            // * is frequency per phase a wanted and supported feature?
            double frequency = paramState.getFreqVal();
            if (frequency == 0.0)
                frequency = 50.0;
            phaseTasks->addSub(TaskGeneratorDspFrequencySet::create(
                m_serverInterface,
                SourceChannelHelper::getChannelMName(type, phaseNo),
                frequency,
                [=]() { qWarning("TaskSetDspFrequency failed for %s", qPrintable(phaseAlias)); }));

            // Amplitudes
            const double peakValue = paramState.getRms(type, phaseNo) * M_SQRT2;
            phaseTasks->addSub(TaskGeneratorRangeByAmplitudeSet::create(
                m_serverInterface,
                SourceChannelHelper::getChannelMName(type, phaseNo),
                peakValue,
                [=]() { qWarning("TaskChangeRangeByAmplitude failed for %s", qPrintable(phaseAlias)); }));
            phaseTasks->addSub(TaskGeneratorDspAmplitudeSet::create(
                m_serverInterface,
                SourceChannelHelper::getChannelMName(type, phaseNo),
                peakValue,
                [=]() { qWarning("TaskSetDspAmplitude failed for %s", qPrintable(phaseAlias)); }));

            // Angles
            phaseTasks->addSub(TaskGeneratorDspAngleSet::create(
                m_serverInterface,
                SourceChannelHelper::getChannelMName(type, phaseNo),
                paramState.getAngle(type, phaseNo),
                [=]() { qWarning("TaskSetDspAmplitude failed for %s", qPrintable(phaseAlias)); }));

            parallelPhaseTasks->addSub(std::move(phaseTasks));
        }
    }
    return parallelPhaseTasks;
}

TaskTemplatePtr SourceSwitchJsonInternal::createSourceModeOnTask(const JsonParamApi &paramState)
{
    QStringList channelMNamesOn = SourceChannelHelper::getChannelMNamesSwitchedOn(m_sourceCapabilities, paramState);
    return TaskGeneratorMultiplePhasesSourceModeOnSet::create(
        m_serverInterface,
        channelMNamesOn,
        [=]() { qWarning("TaskChangeRangeByAmplitude failed for %s", qPrintable(channelMNamesOn.join(","))); }
        );
}

TaskTemplatePtr SourceSwitchJsonInternal::createSourceOnOffTask(const JsonParamApi &paramState)
{
    QStringList channelMNamesOn = SourceChannelHelper::getChannelMNamesSwitchedOn(m_sourceCapabilities, paramState);
    return TaskGeneratorMultiplePhasesSwitchOnSet::create(
        m_serverInterface,
        channelMNamesOn,
        [=]() { qWarning("TaskChangeRangeByAmplitude failed for %s", qPrintable(channelMNamesOn.join(","))); }
        );
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
    if (ok)
        m_paramsCurrent = m_paramsRequested;
    emit sigSwitchFinished();
}


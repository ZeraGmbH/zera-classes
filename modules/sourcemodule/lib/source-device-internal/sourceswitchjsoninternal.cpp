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
    m_loadpointCurrent(m_persistentParamState.loadJsonState()),
    m_loadpointRequestedLast(m_loadpointCurrent)
{
}

int SourceSwitchJsonInternal::switchState(const JsonParamApi &desiredLoad)
{
    TaskTemplatePtr nextTask = desiredLoad.getOn() ?
                                             getSwitchOnTask(desiredLoad) :
                                             getSwitchOffTask(desiredLoad);

    int taskId = nextTask->getTaskId();
    if (m_pendingTask == nullptr)
        doStartTask(std::move(nextTask), desiredLoad);
    else {
        std::shared_ptr<PendingSwitchEntries> queueEntry = std::make_shared<PendingSwitchEntries>();
        queueEntry->desiredLoad = desiredLoad;
        queueEntry->task = std::move(nextTask);
        m_pendingSwitchRequests.enqueue(queueEntry);
    }
    return taskId;
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

void SourceSwitchJsonInternal::doStartTask(TaskTemplatePtr task, const JsonParamApi &desiredLoad)
{
    m_pendingTask = std::move(task);
    m_loadpointRequestedLast = desiredLoad;

    connect(m_pendingTask.get(), &TaskTemplate::sigFinish,
            this, &SourceSwitchJsonInternal::onSwitchTasksFinish);
    m_pendingTask->start();
}

JsonParamApi SourceSwitchJsonInternal::getCurrLoadpoint()
{
    return m_loadpointCurrent;
}

void SourceSwitchJsonInternal::onSwitchTasksFinish(bool ok, int taskId)
{
    m_pendingTask.reset();
    if (ok)
        m_loadpointCurrent = m_loadpointRequestedLast;
    emit sigSwitchFinished(ok, taskId);

    if (m_pendingSwitchRequests.size() != 0) {
        std::shared_ptr<PendingSwitchEntries> nextEntry = m_pendingSwitchRequests.dequeue();
        doStartTask(std::move(nextEntry->task), nextEntry->desiredLoad);
    }
}

TaskTemplatePtr SourceSwitchJsonInternal::getSwitchOnTask(const JsonParamApi &desiredLoad)
{
    TaskContainerInterfacePtr nextTask = TaskContainerSequence::create(TaskContainerSequence::StopOnFirstTaskFail);
    nextTask->addSub(createSourceModeOnTask(desiredLoad));
    nextTask->addSub(createLoadpointTasks(desiredLoad));
    nextTask->addSub(createSourceOnOffTask(desiredLoad));
    return nextTask;
}

TaskTemplatePtr SourceSwitchJsonInternal::getSwitchOffTask(const JsonParamApi &desiredLoad)
{
    JsonParamApi paramOff(desiredLoad);
    paramOff.setOn(false);

    TaskContainerInterfacePtr nextTask = TaskContainerSequence::create(TaskContainerSequence::StopOnFirstTaskFail);
    nextTask->addSub(createSourceOnOffTask(paramOff));
    nextTask->addSub(createSourceModeOnTask(paramOff));
    return nextTask;
}

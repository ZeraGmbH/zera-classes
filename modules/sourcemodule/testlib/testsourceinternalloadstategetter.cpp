#include "testsourceinternalloadstategetter.h"
#include "taskgeneratordspamplitudeget.h"
#include "taskgeneratordspangleget.h"
#include "taskgeneratormultiplephasessourcemodeonget.h"
#include "taskgeneratormultiplephasesswitchonget.h"
#include "taskgeneratordspfrequencyget.h"
#include "sourcechannelhelper.h"
#include <timemachineobject.h>

TestSourceInternalLoadStateGetter::TestSourceInternalLoadStateGetter(AbstractServerInterfacePtr serverInterface) :
    m_serverInterface(serverInterface)
{
}

bool TestSourceInternalLoadStateGetter::getOn(phaseType type, int phaseIdxBase0) const
{
    std::shared_ptr<QStringList> channelOnList = std::make_shared<QStringList>();
    TaskTemplatePtr task = TaskGeneratorMultiplePhasesSwitchOnGet::create(
        m_serverInterface,
        channelOnList);
    task->start();
    TimeMachineObject::feedEventLoop();
    return channelOnList->contains(SourceChannelHelper::getChannelMName(type, phaseIdxBase0));
}

bool TestSourceInternalLoadStateGetter::getSourceModeOn(phaseType type, int phaseIdxBase0) const
{
    std::shared_ptr<QStringList> channelOnList = std::make_shared<QStringList>();
    TaskTemplatePtr task = TaskGeneratorMultiplePhasesSourceModeOnGet::create(
        m_serverInterface,
        channelOnList);
    task->start();
    TimeMachineObject::feedEventLoop();
    return channelOnList->contains(SourceChannelHelper::getChannelMName(type, phaseIdxBase0));
}

double TestSourceInternalLoadStateGetter::getDspPeakAmplitude(phaseType type, int phaseIdxBase0) const
{
    std::shared_ptr<double> value = std::make_shared<double>();
    TaskTemplatePtr task = TaskGeneratorDspAmplitudeGet::create(
        m_serverInterface,
        SourceChannelHelper::getChannelMName(type, phaseIdxBase0),
        value);
    task->start();
    TimeMachineObject::feedEventLoop();
    return *value;
}

double TestSourceInternalLoadStateGetter::getDspAngle(phaseType type, int phaseIdxBase0) const
{
    std::shared_ptr<double> value = std::make_shared<double>();
    TaskTemplatePtr task = TaskGeneratorDspAngleGet::create(
        m_serverInterface,
        SourceChannelHelper::getChannelMName(type, phaseIdxBase0),
        value);
    task->start();
    TimeMachineObject::feedEventLoop();
    return *value;
}

double TestSourceInternalLoadStateGetter::getDspFreqVal(phaseType type, int phaseIdxBase0) const
{
    std::shared_ptr<double> value = std::make_shared<double>();
    TaskTemplatePtr task = TaskGeneratorDspFrequencyGet::create(
        m_serverInterface,
        SourceChannelHelper::getChannelMName(type, phaseIdxBase0),
        value);
    task->start();
    TimeMachineObject::feedEventLoop();
    return *value;
}

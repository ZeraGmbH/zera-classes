#include "taskoffsetsetnode.h"
#include "taskdecoratortimeout.h"
#include <reply.h>
#include <math.h>

TaskTemplatePtr TaskOffsetSetNode::create(Zera::PcbInterfacePtr pcbInterface,
                                           QString channelMName, QString rangeName,
                                           double actualValue, double targetValue, RangeWorkStorage &rngVals,
                                           int timeout, std::function<void()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskOffsetSetNode>(
                                                 pcbInterface,
                                                 channelMName, rangeName,
                                                 actualValue, targetValue, rngVals),
                                             additionalErrorHandler);
}

TaskOffsetSetNode::TaskOffsetSetNode(Zera::PcbInterfacePtr pcbInterface,
                                     QString channelMName, QString rangeName,
                                     double actualValue, double targetValue, RangeWorkStorage &rngVals) :
    m_pcbInterface(pcbInterface),
    m_channelMName(channelMName), m_rangeName(rangeName),
    m_actualValue(actualValue), m_targetValue(targetValue),
    m_rngVals(rngVals)
{
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
            this, &TaskOffsetSetNode::onServerAnswer);
}

void TaskOffsetSetNode::start()
{
    double uncorrectedActualValue = removeGainCorrectionApplied(m_actualValue);
    uncorrectedActualValue = removeOffsetCorrectionApplied(uncorrectedActualValue);

    double correctionAdc = calcAdcOffsetCorrection(uncorrectedActualValue);
    m_msgnr = m_pcbInterface->setOffsetNode(m_channelMName, m_rangeName, 0, correctionAdc, m_targetValue);
}

void TaskOffsetSetNode::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(answer)
    if(m_msgnr == msgnr)
        finishTask(reply == ack);
}

double TaskOffsetSetNode::removeGainCorrectionApplied(double value)
{
    double gainCorrectionApplied = *m_rngVals.m_gainAdjCorrection;
    if (fabs(gainCorrectionApplied) > 1e-9)
        value /= gainCorrectionApplied;
    return value;
}

double TaskOffsetSetNode::removeOffsetCorrectionApplied(double value)
{
    const double nominalAdc = *m_rngVals.m_rejection;
    const double nominalValue = *m_rngVals.m_urValue;
    if (fabs(nominalAdc) > 1e-3) {
        const double offsetCorrectionApplied = *m_rngVals.m_offsetAdjCorrection;
        value = value - offsetCorrectionApplied * nominalValue / nominalAdc;
    }
    return value;
}

double TaskOffsetSetNode::calcAdcOffsetCorrection(double uncorrectedActualValue)
{
    const double nominalAdc = *m_rngVals.m_rejection;
    const double nominalValue = *m_rngVals.m_urValue;
    return (m_targetValue - uncorrectedActualValue) * nominalAdc / nominalValue;
}

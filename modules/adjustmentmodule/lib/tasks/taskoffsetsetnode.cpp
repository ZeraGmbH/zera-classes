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
    double rawActual = m_actualValue;
    const double nominalAdc = *m_rngVals.m_rejection;
    const double nominalValue = *m_rngVals.m_urValue;
    if(fabs(nominalAdc) > 1e-3) {
        // recalc rawActual to uncorrected
        const double currentCorrection = *m_rngVals.m_offsetAdjCorrection;
        rawActual = m_actualValue -
                currentCorrection * nominalValue / nominalAdc;
    }
    double correctionAdc = (m_targetValue - rawActual) * nominalAdc / nominalValue;

    m_msgnr = m_pcbInterface->setOffsetNode(m_channelMName, m_rangeName, 0, correctionAdc, m_targetValue);
}

void TaskOffsetSetNode::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(answer)
    if(m_msgnr == msgnr)
        finishTask(reply == ack);
}

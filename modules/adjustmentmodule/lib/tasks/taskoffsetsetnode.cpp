#include "taskoffsetsetnode.h"
#include "taskdecoratortimeout.h"
#include <reply.h>
#include <math.h>

TaskTemplatePtr TaskOffsetSetNode::create(Zera::PcbInterfacePtr pcbInterface,
                                           QString channelMName, QString rangeName,
                                           double actualValue, double targetValue, RangeVals &rngVals,
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
                                     double actualValue, double targetValue, RangeVals &rngVals) :
    m_pcbInterface(pcbInterface),
    m_channelMName(channelMName), m_rangeName(rangeName),
    m_actualValue(actualValue), m_targetValue(targetValue),
    m_rngVals(rngVals)
{
}

void TaskOffsetSetNode::start()
{
    double rawActual = m_actualValue;
    const double rejection = *m_rngVals.m_rejection;
    const double urValue = *m_rngVals.m_urValue;
    if(fabs(rejection) > 1e-3) {
        rawActual = m_actualValue -
                m_rngVals.m_correction * urValue / rejection;
    }
    double Corr = (m_targetValue - rawActual) * rejection / urValue;

    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
            this, &TaskOffsetSetNode::onServerAnswer);
    m_msgnr = m_pcbInterface->setOffsetNode(m_channelMName, m_rangeName, 0, Corr, m_targetValue);
}

void TaskOffsetSetNode::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack) {
            finishTask(reply == ack);
        }
    }
}

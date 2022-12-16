#include "taskoffsetsetnode.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"
#include <math.h>

TaskCompositePtr TaskOffsetSetNode::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                           QString channelSysName, QString rangeName,
                                           double actualValue, double targetValue, RangeVals &rngVals,
                                           int timeout, std::function<void()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskOffsetSetNode>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 actualValue, targetValue, rngVals),
                                             additionalErrorHandler);
}

TaskOffsetSetNode::TaskOffsetSetNode(Zera::Server::PcbInterfacePtr pcbInterface,
                                     QString channelSysName, QString rangeName,
                                     double actualValue, double targetValue, RangeVals &rngVals) :
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName), m_rangeName(rangeName),
    m_actualValue(actualValue), m_targetValue(targetValue),
    m_rngVals(rngVals)
{
}

void TaskOffsetSetNode::start()
{
    double rawActual = m_actualValue;
    if(fabs(m_rngVals.m_rejection) > 1e-3) {
        rawActual = m_actualValue -
                m_rngVals.m_correction * m_rngVals.m_rejectionValue / m_rngVals.m_rejection;
    }
    double Corr = (m_targetValue - rawActual) * m_rngVals.m_rejection / m_rngVals.m_rejectionValue;

    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer,
            this, &TaskOffsetSetNode::onServerAnswer);
    m_msgnr = m_pcbInterface->setOffsetNode(m_channelSysName, m_rangeName, 0, Corr, m_targetValue);
}

void TaskOffsetSetNode::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack) {
            finishTask(reply == ack);
        }
    }
}

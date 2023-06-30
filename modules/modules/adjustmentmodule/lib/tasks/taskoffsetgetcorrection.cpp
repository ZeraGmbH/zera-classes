#include "taskoffsetgetcorrection.h"
#include "taskdecoratortimeout.h"
#include <reply.h>

TaskTemplatePtr TaskOffsetGetCorrection::create(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName, double ourActualValue,
                                                 double &correctionValue,
                                                 int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskOffsetGetCorrection>(
                                                 pcbInterface,
                                                 channelSysName, rangeName, ourActualValue,
                                                 correctionValue),
                                             additionalErrorHandler);

}

TaskOffsetGetCorrection::TaskOffsetGetCorrection(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName, double ourActualValue,
                                                 double &correctionValue) :
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName), m_rangeName(rangeName), m_ourActualValue(ourActualValue),
    m_correctionValue(correctionValue)
{
}

void TaskOffsetGetCorrection::start()
{
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer,
            this, &TaskOffsetGetCorrection::onServerAnswer);
    m_msgnr = m_pcbInterface->getAdjOffsetCorrection(m_channelSysName, m_rangeName, m_ourActualValue);
}

void TaskOffsetGetCorrection::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack)
            m_correctionValue = answer.toDouble();
        finishTask(reply == ack);
    }
}

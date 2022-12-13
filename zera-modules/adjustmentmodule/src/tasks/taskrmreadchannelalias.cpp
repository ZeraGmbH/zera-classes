#include "taskrmreadchannelalias.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"
#include "adjustmentmodulemeasprogram.h"

TaskRmReadChannelAlias::TaskRmReadChannelAlias(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName, QString &targetAlias) :
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_targetAlias(targetAlias)
{
}

std::unique_ptr<TaskComposite> TaskRmReadChannelAlias::create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                              QString &targetAlias)
{
    return std::make_unique<TaskRmReadChannelAlias>(pcbInterface, channelName, targetAlias);
}

std::unique_ptr<TaskComposite> TaskRmReadChannelAlias::create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                              QString &targetAlias,
                                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(pcbInterface, channelName, targetAlias), additionalErrorHandler);
}

void TaskRmReadChannelAlias::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskRmReadChannelAlias::onRmAnswer);
    m_msgnr = m_pcbInterface->getAlias(m_channelName);
}

void TaskRmReadChannelAlias::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack) {
            QString alias = answer.toString();
            m_targetAlias = alias;
            finishTask(true);
        }
        else
            finishTask(false);
    }
}

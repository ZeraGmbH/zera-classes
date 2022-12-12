#include "taskrmreadchannelalias.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"
#include "adjustmentmodulemeasprogram.h"

namespace ADJUSTMENTMODULE {

TaskRmReadChannelAlias::TaskRmReadChannelAlias(AdjustmentModuleActivateDataPtr activationData, QString channelName) :
    m_activationData(activationData),
    m_channelName(channelName)
{
}

std::unique_ptr<TaskComposite> TaskRmReadChannelAlias::create(AdjustmentModuleActivateDataPtr activationData, QString channelName)
{
    return std::make_unique<TaskRmReadChannelAlias>(activationData, channelName);
}

std::unique_ptr<TaskComposite> TaskRmReadChannelAlias::create(AdjustmentModuleActivateDataPtr activationData, QString channelName, int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(activationData, channelName), additionalErrorHandler);
}

void TaskRmReadChannelAlias::start()
{
    connect(m_activationData->m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskRmReadChannelAlias::onRmAnswer);
    m_msgnr = m_activationData->m_pcbInterface->getAlias(m_channelName);
}

void TaskRmReadChannelAlias::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack) {
            QString alias = answer.toString();
            m_activationData->m_AliasChannelHash[alias] = m_channelName;
            m_activationData->m_adjustChannelInfoHash[m_channelName]->m_sAlias = alias;
            finishTask(true);
        }
        else
            finishTask(false);
    }
}
}

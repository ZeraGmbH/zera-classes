#include "taskchannelreadranges.h"
#include "adjustmentmodulemeasprogram.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

namespace ADJUSTMENTMODULE {

std::unique_ptr<TaskComposite> TaskChannelReadRanges::create(AdjustmentModuleActivateDataPtr activationData, QString channelName)
{
    return std::make_unique<TaskChannelReadRanges>(activationData, channelName);
}

std::unique_ptr<TaskComposite> TaskChannelReadRanges::create(AdjustmentModuleActivateDataPtr activationData, QString channelName,
                                                             int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(activationData, channelName), additionalErrorHandler);
}

TaskChannelReadRanges::TaskChannelReadRanges(AdjustmentModuleActivateDataPtr activationData, QString channelName) :
    m_activationData(activationData),
    m_channelName(channelName)
{
}

void TaskChannelReadRanges::start()
{
    connect(m_activationData->m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelReadRanges::onRmAnswer);
    m_msgnr = m_activationData->m_pcbInterface->getRangeList(m_channelName);
}

void TaskChannelReadRanges::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(msgnr == m_msgnr) {
        if (reply == ack)
            m_activationData->m_adjustChannelInfoHash[m_channelName]->m_sRangelist = answer.toStringList();
        finishTask(reply == ack);
    }
}

}

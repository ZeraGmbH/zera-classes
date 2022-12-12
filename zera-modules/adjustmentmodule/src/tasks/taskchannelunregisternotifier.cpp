#include "taskchannelunregisternotifier.h"
#include "adjustmentmodulemeasprogram.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

namespace ADJUSTMENTMODULE {

std::unique_ptr<TaskComposite> TaskChannelUnregisterNotifier::create(AdjustmentModuleCommonPtr activationData, QString channelName)
{
    return std::make_unique<TaskChannelUnregisterNotifier>(activationData, channelName);
}

std::unique_ptr<TaskComposite> TaskChannelUnregisterNotifier::create(AdjustmentModuleCommonPtr activationData, QString channelName,
                                                                   int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(activationData, channelName), additionalErrorHandler);
}

TaskChannelUnregisterNotifier::TaskChannelUnregisterNotifier(AdjustmentModuleCommonPtr activationData, QString channelName) :
    m_commonObjects(activationData),
    m_channelName(channelName)
{
}

void TaskChannelUnregisterNotifier::start()
{
    connect(m_commonObjects->m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelUnregisterNotifier::onRmAnswer);
    m_msgnr = m_commonObjects->m_pcbInterface->unregisterNotifiers();
}

void TaskChannelUnregisterNotifier::onRmAnswer(quint32 msgnr, quint8 reply, QVariant)
{
    if(msgnr == m_msgnr) {
        finishTask(reply == ack);
    }
}

}

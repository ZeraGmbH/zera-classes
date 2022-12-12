#include "taskchannelregisternotifier.h"
#include "adjustmentmodulemeasprogram.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

namespace ADJUSTMENTMODULE {

std::unique_ptr<TaskComposite> TaskChannelRegisterNotifier::create(AdjustmentModuleActivateDataPtr activationData, QString channelName)
{
    return std::make_unique<TaskChannelRegisterNotifier>(activationData, channelName);
}

std::unique_ptr<TaskComposite> TaskChannelRegisterNotifier::create(AdjustmentModuleActivateDataPtr activationData, QString channelName,
                                                                   int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(activationData, channelName), additionalErrorHandler);
}

TaskChannelRegisterNotifier::TaskChannelRegisterNotifier(AdjustmentModuleActivateDataPtr activationData, QString channelName) :
    m_activationData(activationData),
    m_channelName(channelName)
{
}

void TaskChannelRegisterNotifier::start()
{
    connect(m_activationData->m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelRegisterNotifier::onRmAnswer);
    m_msgnr = m_activationData->m_pcbInterface->registerNotifier(QString("SENS:%1:RANG:CAT?").arg(m_channelName), "1");
}

void TaskChannelRegisterNotifier::onRmAnswer(quint32 msgnr, quint8 reply, QVariant)
{
    if(msgnr == m_msgnr) {
        finishTask(reply == ack);
    }
}

}

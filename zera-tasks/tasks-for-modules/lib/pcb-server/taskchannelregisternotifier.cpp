#include "taskchannelregisternotifier.h"
#include "tasktimeoutdecorator.h"

TaskCompositePtr TaskChannelRegisterNotifier::create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                     int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelRegisterNotifier>(
                                                 pcbInterface,
                                                 channelName),
                                             additionalErrorHandler);
}

TaskChannelRegisterNotifier::TaskChannelRegisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface,
                                                         QString channelName) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelName(channelName)
{
}

quint32 TaskChannelRegisterNotifier::sendToServer()
{
    return m_pcbInterface->registerNotifier(QString("SENS:%1:RANG:CAT?").arg(m_channelName), 1);
}

bool TaskChannelRegisterNotifier::handleCheckedServerAnswer(QVariant answer)
{
    return true;
}

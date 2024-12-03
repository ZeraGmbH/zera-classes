#include "taskgetsamplerate.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskGetSampleRate::create(Zera::PcbInterfacePtr pcbInterface,
                                            int &valueReceived,
                                            int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskGetSampleRate>(
                                                 pcbInterface,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskGetSampleRate::TaskGetSampleRate(Zera::PcbInterfacePtr pcbInterface, int &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_valueReceived(valueReceived)
{
}

quint32 TaskGetSampleRate::sendToServer()
{
    return m_pcbInterface->getSampleRate();
}

bool TaskGetSampleRate::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    m_valueReceived = answer.toInt(&ok);
    return ok;
}

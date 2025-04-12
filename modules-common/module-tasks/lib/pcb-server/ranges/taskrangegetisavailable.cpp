#include "taskrangegetisavailable.h"
#include "taskgetscpibool.h"

TaskTemplatePtr TaskRangeGetIsAvailable::create(Zera::PcbInterfacePtr pcbInterface,
                                                QString channelMName, QString rangeName,
                                                std::shared_ptr<bool> valueReceived,
                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskGetScpiBool::create(pcbInterface,
                                   QString("SENS:%1:%2:AVA?").arg(channelMName, rangeName),
                                   valueReceived,
                                   timeout,
                                   additionalErrorHandler);
}

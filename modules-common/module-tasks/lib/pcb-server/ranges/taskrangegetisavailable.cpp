#include "taskrangegetisavailable.h"
#include "taskscpigetbool.h"

TaskTemplatePtr TaskRangeGetIsAvailable::create(Zera::PcbInterfacePtr pcbInterface,
                                                QString channelMName, QString rangeName,
                                                std::shared_ptr<bool> valueReceived,
                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetBool::create(pcbInterface,
                                   QString("SENS:%1:%2:AVA?").arg(channelMName, rangeName),
                                   valueReceived,
                                   timeout,
                                   additionalErrorHandler);
}

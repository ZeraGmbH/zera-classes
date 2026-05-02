#include "taskrangegetisavailable.h"
#include "taskscpigetbool.h"

TaskTemplatePtr TaskRangeGetIsAvailable::create(const Zera::PcbInterfacePtr &pcbInterface,
                                                const QString &channelMName, const QString &rangeName,
                                                std::shared_ptr<bool> valueReceived,
                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetBool::create(pcbInterface,
                                   QString("SENS:%1:%2:AVA?").arg(channelMName, rangeName),
                                   valueReceived,
                                   timeout,
                                   additionalErrorHandler);
}

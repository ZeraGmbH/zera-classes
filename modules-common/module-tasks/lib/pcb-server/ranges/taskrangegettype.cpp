#include "taskrangegettype.h"
#include "taskscpigetint.h"

TaskTemplatePtr TaskRangeGetType::create(const Zera::PcbInterfacePtr &pcbInterface,
                                         const QString &channelMName, const QString &rangeName,
                                         std::shared_ptr<int> valueReceived,
                                         int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetInt::create(pcbInterface,
                                  QString("SENSE:%1:%2:TYPE?").arg(channelMName, rangeName),
                                  valueReceived,
                                  timeout,
                                  additionalErrorHandler);
}

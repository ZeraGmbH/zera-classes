#include "taskrangegetrejection.h"
#include "taskscpigetdouble.h"

TaskTemplatePtr TaskRangeGetRejection::create(const Zera::PcbInterfacePtr &pcbInterface,
                                              const QString &channelMName, const QString &rangeName,
                                              std::shared_ptr<double> valueReceived,
                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetDouble::create(pcbInterface,
                                     QString("SENSE:%1:%2:REJECTION?").arg(channelMName, rangeName),
                                     valueReceived,
                                     timeout,
                                     additionalErrorHandler);
}

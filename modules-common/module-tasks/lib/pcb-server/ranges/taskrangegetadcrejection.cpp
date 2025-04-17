#include "taskrangegetadcrejection.h"
#include "taskscpigetdouble.h"

TaskTemplatePtr TaskRangeGetAdcRejection::create(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelMName, QString rangeName,
                                                 std::shared_ptr<double> valueReceived,
                                                 int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetDouble::create(pcbInterface,
                                     QString("SENSE:%1:%2:ADCREJECTION?").arg(channelMName, rangeName),
                                     valueReceived,
                                     timeout,
                                     additionalErrorHandler);
}

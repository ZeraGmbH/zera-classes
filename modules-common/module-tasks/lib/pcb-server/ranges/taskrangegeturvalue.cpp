#include "taskrangegeturvalue.h"
#include "taskscpigetdouble.h"

TaskTemplatePtr TaskRangeGetUrValue::create(Zera::PcbInterfacePtr pcbInterface,
                                            QString channelMName, QString rangeName,
                                            std::shared_ptr<double> valueReceived,
                                            int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetDouble::create(pcbInterface,
                                     QString("SENSE:%1:%2:URVALUE?").arg(channelMName, rangeName),
                                     valueReceived,
                                     timeout,
                                     additionalErrorHandler);
}

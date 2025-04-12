#include "taskrangegetrejection.h"
#include "taskgetscpidouble.h"

TaskTemplatePtr TaskRangeGetRejection::create(Zera::PcbInterfacePtr pcbInterface,
                                              QString channelMName, QString rangeName,
                                              std::shared_ptr<double> valueReceived,
                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskGetScpiDouble::create(pcbInterface,
                                     QString("SENSE:%1:%2:REJECTION?").arg(channelMName, rangeName),
                                     valueReceived,
                                     timeout,
                                     additionalErrorHandler);
}

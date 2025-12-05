#include "taskgaingetadjcorrection.h"
#include "taskscpigetdouble.h"

TaskTemplatePtr TaskGainGetAdjCorrection::create(AbstractServerInterfacePtr pcbInterface,
                                                 QString channelMName, QString rangeName, double ourActualValue,
                                                 std::shared_ptr<double> valueReceived,
                                                 int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetDouble::create(pcbInterface,
                                     QString("SENSE:%1:%2:CORR:ADJGAIN? %3;").arg(channelMName, rangeName).arg(ourActualValue),
                                     valueReceived,
                                     timeout,
                                     additionalErrorHandler);
}

#include "taskgeneratordspangleget.h"
#include "taskscpigetdouble.h"

TaskTemplatePtr TaskGeneratorDspAngleGet::create(AbstractServerInterfacePtr interface,
                                                 const QString &channelMName,
                                                 std::shared_ptr<double> angleReceived,
                                                 std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:%1:DSANGLE?").arg(channelMName);
    return TaskScpiGetDouble::create(interface,
                                     cmd,
                                     angleReceived,
                                     timeout,
                                     additionalErrorHandler);
}

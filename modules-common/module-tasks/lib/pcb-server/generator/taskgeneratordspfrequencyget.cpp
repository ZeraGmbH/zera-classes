#include "taskgeneratordspfrequencyget.h"
#include "taskscpigetdouble.h"

TaskTemplatePtr TaskGeneratorDspFrequencyGet::create(AbstractServerInterfacePtr interface,
                                                     const QString &channelMName,
                                                     std::shared_ptr<double> frequencyReceived,
                                                     std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:%1:DSFREQUENCY?").arg(channelMName);
    return TaskScpiGetDouble::create(interface,
                                     cmd,
                                     frequencyReceived,
                                     timeout,
                                     additionalErrorHandler);
}

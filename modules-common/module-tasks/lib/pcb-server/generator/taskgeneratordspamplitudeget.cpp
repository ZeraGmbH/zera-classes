#include "taskgeneratordspamplitudeget.h"
#include "taskscpigetdouble.h"

TaskTemplatePtr TaskGeneratorDspAmplitudeGet::create(AbstractServerInterfacePtr interface,
                                                     const QString &channelMName,
                                                     std::shared_ptr<double> amplitudeReceived,
                                                     std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:%1:DSAMPLITUDE?").arg(channelMName);
    return TaskScpiGetDouble::create(interface,
                                  cmd,
                                  amplitudeReceived,
                                  timeout,
                                  additionalErrorHandler);

}

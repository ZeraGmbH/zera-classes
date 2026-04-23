#include "tasksetdspamplitude.h"

TaskTemplatePtr TaskSetDspAmplitude::create(AbstractServerInterfacePtr interface,
                                            const QString &channelMName, float amplitude,
                                            std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:%1:DSAMPLITUDE %2;").arg(channelMName).arg(amplitude);
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

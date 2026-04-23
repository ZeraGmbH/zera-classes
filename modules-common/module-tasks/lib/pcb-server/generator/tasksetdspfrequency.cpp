#include "tasksetdspfrequency.h"

TaskTemplatePtr TaskSetDspFrequency::create(AbstractServerInterfacePtr interface,
                                            const QString &channelMName, float frequency,
                                            std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:%1:DSFREQUENCY %2;").arg(channelMName).arg(frequency);
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

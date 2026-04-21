#include "tasksetdspfrequency.h"

TaskTemplatePtr TaskSetDspFrequency::create(AbstractServerInterfacePtr interface,
                                            const QString &channelMName, float frequency,
                                            int timeout, std::function<void ()> additionalErrorHandler)
{
    QString cmd = QString("GENERATOR:%1:DSFREQUENCY %2;").arg(channelMName).arg(frequency);
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

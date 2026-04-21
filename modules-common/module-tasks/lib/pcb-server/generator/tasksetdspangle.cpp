#include "tasksetdspangle.h"

TaskTemplatePtr TaskSetDspAngle::create(AbstractServerInterfacePtr interface,
                                        const QString &channelMName, float angle,
                                        int timeout, std::function<void ()> additionalErrorHandler)
{
    QString cmd = QString("GENERATOR:%1:DSANGLE %2;").arg(channelMName).arg(angle);
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

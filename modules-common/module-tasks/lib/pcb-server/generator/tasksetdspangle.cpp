#include "tasksetdspangle.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskSetDspAngle::create(AbstractServerInterfacePtr interface,
                                        const QString &channelMName, float angle,
                                        std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:%1:DSANGLE %2;").arg(channelMName).arg(angle);
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

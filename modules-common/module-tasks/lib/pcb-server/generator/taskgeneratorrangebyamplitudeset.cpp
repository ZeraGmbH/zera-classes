#include "taskgeneratorrangebyamplitudeset.h"
#include <taskscpicmd.h>

TaskTemplatePtr TaskGeneratorRangeByAmplitudeSet::create(AbstractServerInterfacePtr interface,
                                                   const QString &channelMName,
                                                   float amplitude,
                                                   std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:%1:AMPRANGE %2;").arg(channelMName).arg(amplitude);
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

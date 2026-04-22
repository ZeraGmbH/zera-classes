#include "taskgeneratormultiplephasesswitchon.h"

TaskTemplatePtr TaskGeneratorMultiplePhasesSwitchOn::create(AbstractServerInterfacePtr interface,
                                                            const QStringList &channelMNameListOn,
                                                            int timeout, std::function<void ()> additionalErrorHandler)
{
    QString cmd = QString("GENERATOR:SWITCHON %1;").arg(channelMNameListOn.join(","));
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

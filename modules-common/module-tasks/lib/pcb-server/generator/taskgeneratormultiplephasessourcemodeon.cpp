#include "taskgeneratormultiplephasessourcemodeon.h"

TaskTemplatePtr TaskGeneratorMultiplePhasesSourceModeOn::create(AbstractServerInterfacePtr interface,
                                                                const QStringList &channelMNameListOn,
                                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    QString cmd = QString("GENERATOR:MODEON %1;").arg(channelMNameListOn.join(","));
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

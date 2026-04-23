#include "taskgeneratormultiplephasessourcemodeon.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskGeneratorMultiplePhasesSourceModeOn::create(AbstractServerInterfacePtr interface,
                                                                const QStringList &channelMNameListOn,
                                                                std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:MODEON %1;").arg(channelMNameListOn.join(","));
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

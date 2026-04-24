#include "taskgeneratormultiplephasesswitchonset.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskGeneratorMultiplePhasesSwitchOnSet::create(AbstractServerInterfacePtr interface,
                                                            const QStringList &channelMNameListOn,
                                                            std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = QString("GENERATOR:SWITCHON %1;").arg(channelMNameListOn.join(","));
    return TaskScpiCmd::create(interface,
                               cmd,
                               timeout,
                               additionalErrorHandler);
}

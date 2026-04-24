#include "taskgeneratormultiplephasesswitchonget.h"
#include "taskscpigetstringlist.h"

TaskTemplatePtr TaskGeneratorMultiplePhasesSwitchOnGet::create(AbstractServerInterfacePtr interface,
                                                               std::shared_ptr<QStringList> channelMNameListOn,
                                                               std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = "GENERATOR:SWITCHON?";
    return TaskScpiGetStringList::create(interface,
                                         cmd,
                                         channelMNameListOn, ",",
                                         timeout,
                                         additionalErrorHandler);
}

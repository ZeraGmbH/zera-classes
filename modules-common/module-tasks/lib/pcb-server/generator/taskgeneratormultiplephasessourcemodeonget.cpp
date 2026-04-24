#include "taskgeneratormultiplephasessourcemodeonget.h"
#include "taskscpigetstringlist.h"

TaskTemplatePtr TaskGeneratorMultiplePhasesSourceModeOnGet::create(AbstractServerInterfacePtr interface,
                                                                   std::shared_ptr<QStringList> channelMNameListOn,
                                                                   std::function<void ()> additionalErrorHandler, int timeout)
{
    QString cmd = "GENERATOR:MODEON?";
    return TaskScpiGetStringList::create(interface,
                                         cmd,
                                         channelMNameListOn, ",",
                                         timeout,
                                         additionalErrorHandler);
}

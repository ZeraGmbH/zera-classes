#include "taskchannelgetavail.h"
#include "taskgetscpistringlist.h"

TaskTemplatePtr TaskChannelGetAvail::create(Zera::PcbInterfacePtr pcbInterface,
                                            std::shared_ptr<QStringList> channelsReceived,
                                            int timeout,
                                            std::function<void ()> additionalErrorHandler)
{
    return TaskGetScpiStringList::create(pcbInterface,
                                         "SENSE:CHANNEL:CAT?",
                                         channelsReceived,
                                         timeout,
                                         additionalErrorHandler);
}

#include "taskchannelgetavail.h"
#include "taskscpigetstringlist.h"

TaskTemplatePtr TaskChannelGetAvail::create(Zera::PcbInterfacePtr pcbInterface,
                                            std::shared_ptr<QStringList> channelsReceived,
                                            int timeout,
                                            std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetStringList::create(pcbInterface,
                                         "SENSE:CHANNEL:CAT?",
                                         channelsReceived,
                                         timeout,
                                         additionalErrorHandler);
}

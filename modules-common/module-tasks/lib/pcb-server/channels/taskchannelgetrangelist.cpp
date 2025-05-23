#include "taskchannelgetrangelist.h"
#include "taskscpigetstringlist.h"

TaskTemplatePtr TaskChannelGetRangeList::create(Zera::PcbInterfacePtr pcbInterface,
                                                QString channelName,
                                                std::shared_ptr<QStringList> rangeList,
                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetStringList::create(pcbInterface,
                                         QString("SENS:%1:RANG:CAT?").arg(channelName),
                                         rangeList,
                                         timeout,
                                         additionalErrorHandler);
}

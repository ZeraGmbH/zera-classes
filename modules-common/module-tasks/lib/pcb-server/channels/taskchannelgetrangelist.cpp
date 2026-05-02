#include "taskchannelgetrangelist.h"
#include "taskscpigetstringlist.h"

TaskTemplatePtr TaskChannelGetRangeList::create(const Zera::PcbInterfacePtr &pcbInterface,
                                                const QString &channelName,
                                                std::shared_ptr<QStringList> rangeList,
                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetStringList::create(pcbInterface,
                                         QString("SENS:%1:RANG:CAT?").arg(channelName),
                                         rangeList, ";",
                                         timeout,
                                         additionalErrorHandler);
}

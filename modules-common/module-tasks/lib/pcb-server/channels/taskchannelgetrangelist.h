#ifndef TASKCHANNELREADRANGES_H
#define TASKCHANNELREADRANGES_H

#include <tasktemplate.h>
#include <pcbinterface.h>

class TaskChannelGetRangeList
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelName,
                                  std::shared_ptr<QStringList> rangeList,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANNELREADRANGES_H

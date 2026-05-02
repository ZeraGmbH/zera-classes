#ifndef TASKCHANNELREADRANGES_H
#define TASKCHANNELREADRANGES_H

#include <tasktemplate.h>
#include <pcbinterface.h>

class TaskChannelGetRangeList
{
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface,
                                  const QString &channelName,
                                  std::shared_ptr<QStringList> rangeList,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANNELREADRANGES_H

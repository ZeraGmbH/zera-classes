#ifndef TASKCHANNELGETDSPCHANNEL_H
#define TASKCHANNELGETDSPCHANNEL_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskChannelGetDspChannel
{
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface, const QString &channelName,
                                  std::shared_ptr<int> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANNELGETDSPCHANNEL_H

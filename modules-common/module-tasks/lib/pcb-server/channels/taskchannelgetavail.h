#ifndef TASKCHANNELGETAVAIL_H
#define TASKCHANNELGETAVAIL_H

#include <tasktemplate.h>
#include <pcbinterface.h>

class TaskChannelGetAvail
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  std::shared_ptr<QStringList> channelsReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANNELGETAVAIL_H

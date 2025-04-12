#ifndef TASKCHANNELGETUNIT_H
#define TASKCHANNELGETUNIT_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskChannelGetUnit
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface, QString channelName,
                                  std::shared_ptr<QString> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANNELGETUNIT_H

#ifndef TASKCHANNELGETUNIT_H
#define TASKCHANNELGETUNIT_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskChannelGetUnit
{
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface, const QString &channelName,
                                  std::shared_ptr<QString> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKCHANNELGETUNIT_H

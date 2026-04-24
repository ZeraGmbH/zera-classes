#ifndef TASKGENERATORMULTIPLEPHASESSWITCHONGET_H
#define TASKGENERATORMULTIPLEPHASESSWITCHONGET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorMultiplePhasesSwitchOnGet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  std::shared_ptr<QStringList> channelMNameListOn, // those no mentioned are switched off
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORMULTIPLEPHASESSWITCHONGET_H

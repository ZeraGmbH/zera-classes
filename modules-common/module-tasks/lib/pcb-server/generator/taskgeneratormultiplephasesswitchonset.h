#ifndef TASKGENERATORMULTIPLEPHASESSWITCHONSET_H
#define TASKGENERATORMULTIPLEPHASESSWITCHONSET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorMultiplePhasesSwitchOnSet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QStringList &channelMNameListOn, // those no mentioned are switched off
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORMULTIPLEPHASESSWITCHONSET_H

#ifndef TASKGENERATORMULTIPLEPHASESSOURCEMODEONSET_H
#define TASKGENERATORMULTIPLEPHASESSOURCEMODEONSET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorMultiplePhasesSourceModeOnSet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QStringList &channelMNameListOn, // those no mentioned are switched off
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORMULTIPLEPHASESSOURCEMODEONSET_H

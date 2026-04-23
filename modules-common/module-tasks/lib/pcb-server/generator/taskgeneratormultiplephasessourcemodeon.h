#ifndef TASKGENERATORMULTIPLEPHASESSOURCEMODEON_H
#define TASKGENERATORMULTIPLEPHASESSOURCEMODEON_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorMultiplePhasesSourceModeOn
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QStringList &channelMNameListOn, // those no mentioned are switched off
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORMULTIPLEPHASESSOURCEMODEON_H

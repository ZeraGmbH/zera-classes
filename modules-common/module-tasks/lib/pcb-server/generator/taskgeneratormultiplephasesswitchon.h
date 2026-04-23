#ifndef TASKGENERATORMULTIPLEPHASESSWITCHON_H
#define TASKGENERATORMULTIPLEPHASESSWITCHON_H

#include "taskscpicmd.h"

class TaskGeneratorMultiplePhasesSwitchOn : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QStringList &channelMNameListOn, // those no mentioned are switched off
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORMULTIPLEPHASESSWITCHON_H

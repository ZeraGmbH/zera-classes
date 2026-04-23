#ifndef TASKGENERATORMULTIPLEPHASESSWITCHON_H
#define TASKGENERATORMULTIPLEPHASESSWITCHON_H

#include "taskscpicmd.h"

class TaskGeneratorMultiplePhasesSwitchOn : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QStringList &channelMNameListOn, // those no mentioned are switched off
                                  int timeout = TRANSACTION_TIMEOUT, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKGENERATORMULTIPLEPHASESSWITCHON_H

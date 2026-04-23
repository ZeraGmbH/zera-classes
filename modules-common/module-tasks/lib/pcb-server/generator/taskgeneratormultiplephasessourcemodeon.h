#ifndef TASKGENERATORMULTIPLEPHASESSOURCEMODEON_H
#define TASKGENERATORMULTIPLEPHASESSOURCEMODEON_H

#include "taskscpicmd.h"

class TaskGeneratorMultiplePhasesSourceModeOn : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QStringList &channelMNameListOn, // those no mentioned are switched off
                                  int timeout = TRANSACTION_TIMEOUT, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKGENERATORMULTIPLEPHASESSOURCEMODEON_H

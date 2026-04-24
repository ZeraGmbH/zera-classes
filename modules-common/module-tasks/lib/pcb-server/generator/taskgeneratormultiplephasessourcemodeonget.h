#ifndef TASKGENERATORMULTIPLEPHASESSOURCEMODEONGET_H
#define TASKGENERATORMULTIPLEPHASESSOURCEMODEONGET_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGeneratorMultiplePhasesSourceModeOnGet
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  std::shared_ptr<QStringList> channelMNameListOn, // those no mentioned are switched off
                                  std::function<void()> additionalErrorHandler = []{}, int timeout = TRANSACTION_TIMEOUT);
};

#endif // TASKGENERATORMULTIPLEPHASESSOURCEMODEONGET_H

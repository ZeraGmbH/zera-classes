#ifndef TASKSETDSPAMPLITUDE_H
#define TASKSETDSPAMPLITUDE_H

#include "taskscpicmd.h"

class TaskSetDspAmplitude : public TaskScpiCmd
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &channelMName, float amplitude,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKSETDSPAMPLITUDE_H

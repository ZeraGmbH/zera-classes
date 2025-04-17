#ifndef TASKREMOVECMDLISTSFORALLCLIENTS_H
#define TASKREMOVECMDLISTSFORALLCLIENTS_H

#include <dspinterface.h>
#include <tasktemplate.h>

class TaskRemoveCmdListsForAllClients
{
public:
    static TaskTemplatePtr create(Zera::DspInterfacePtr dspInterface,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKREMOVECMDLISTSFORALLCLIENTS_H

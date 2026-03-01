#include "taskremovecmdlistsforallclients.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskRemoveCmdListsForAllClients::create(Zera::DspInterfacePtr dspInterface,
                                                        int timeout,
                                                        std::function<void ()> additionalErrorHandler)
{
    return TaskScpiCmd::create(dspInterface,
                               "MEMORY:CLALL",
                               timeout,
                               additionalErrorHandler);
}

#include "taskemobactivatepushbutton.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskEmobActivatePushButton::create(Zera::PcbInterfacePtr pcbInterface,
                                                   int timeout,
                                                   std::function<void ()> additionalErrorHandler)
{
    return TaskScpiCmd::create(pcbInterface,
                        "SYST:EMOB:PBPR;",
                        timeout,
                        additionalErrorHandler);
}

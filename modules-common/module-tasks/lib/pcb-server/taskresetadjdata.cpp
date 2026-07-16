#include "taskresetadjdata.h"
#include "taskscpicmd.h"

TaskTemplatePtr TaskResetAdjData::create(Zera::PcbInterfacePtr pcbInterface, int timeout,
                                         std::function<void ()> additionalErrorHandler)
{
    QString cmd = "SENSE:RESET:ADJUSTMENT;";
    return TaskScpiCmd::create(pcbInterface,
                               cmd,
                               timeout,
                               additionalErrorHandler);

}

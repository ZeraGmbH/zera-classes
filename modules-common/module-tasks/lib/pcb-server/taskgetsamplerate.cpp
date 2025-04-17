#include "taskgetsamplerate.h"
#include "taskscpigetint.h"

TaskTemplatePtr TaskGetSampleRate::create(Zera::PcbInterfacePtr pcbInterface,
                                          std::shared_ptr<int> valueReceived,
                                          int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetInt::create(pcbInterface,
                                  "SAMPLE:SRATE?",
                                  valueReceived,
                                  timeout,
                                  additionalErrorHandler);
}

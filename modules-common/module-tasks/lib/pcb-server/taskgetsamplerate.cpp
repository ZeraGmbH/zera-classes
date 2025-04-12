#include "taskgetsamplerate.h"
#include "taskgetscpiint.h"

TaskTemplatePtr TaskGetSampleRate::create(Zera::PcbInterfacePtr pcbInterface,
                                          std::shared_ptr<int> valueReceived,
                                          int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskGetScpiInt::create(pcbInterface,
                                  "SAMPLE:SRATE?",
                                  valueReceived,
                                  timeout,
                                  additionalErrorHandler);
}

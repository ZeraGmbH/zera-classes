#include "taskpllchannelget.h"
#include "taskscpigetstring.h"

TaskTemplatePtr TaskPllChannelGet::create(AbstractServerInterfacePtr pcbInterface,
                                          std::shared_ptr<QString> &pllChannelMNameReceived,
                                          int timeout,
                                          std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetString::create(pcbInterface,
                                     QString("SAMPLE:S0:PLL?"),
                                     pllChannelMNameReceived,
                                     timeout,
                                     additionalErrorHandler);
}

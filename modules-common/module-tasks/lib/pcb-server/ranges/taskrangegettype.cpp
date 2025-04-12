#include "taskrangegettype.h"
#include "taskgetscpiint.h"

TaskTemplatePtr TaskRangeGetType::create(Zera::PcbInterfacePtr pcbInterface,
                                         QString channelMName, QString rangeName,
                                         std::shared_ptr<int> valueReceived,
                                         int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskGetScpiInt::create(pcbInterface,
                                  QString("SENSE:%1:%2:TYPE?").arg(channelMName, rangeName),
                                  valueReceived,
                                  timeout,
                                  additionalErrorHandler);
}

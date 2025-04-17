#include "taskrefpowerfetchconstant.h"
#include "taskscpigetdouble.h"

TaskTemplatePtr TaskRefPowerFetchConstant::create(Zera::PcbInterfacePtr pcbInterface,
                                                  QString refPowerName,
                                                  std::shared_ptr<double> refPowerConstantReceived,
                                                  int timeout,
                                                  std::function<void ()> additionalErrorHandler)
{
    return TaskScpiGetDouble::create(pcbInterface,
                                     QString("SOURCE:%1:CONSTANT?").arg(refPowerName),
                                     refPowerConstantReceived,
                                     timeout,
                                     additionalErrorHandler);
}

#ifndef TASKREFPOWERFETCHCONSTANT_H
#define TASKREFPOWERFETCHCONSTANT_H

#include <pcbinterface.h>
#include <tasktemplate.h>

class TaskRefPowerFetchConstant
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString refPowerName, // fo0;fo1...
                                  std::shared_ptr<double> refPowerConstantReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKREFPOWERFETCHCONSTANT_H

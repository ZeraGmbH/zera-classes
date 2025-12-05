#ifndef TASKGAINGETADJCORRECTION_H
#define TASKGAINGETADJCORRECTION_H

#include <abstractserverInterface.h>
#include <tasktemplate.h>

class TaskGainGetAdjCorrection
{
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName, double ourActualValue,
                                  std::shared_ptr<double> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKGAINGETADJCORRECTION_H

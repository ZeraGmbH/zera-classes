#ifndef TASKOFFSETGETADJCORRECTION_H
#define TASKOFFSETGETADJCORRECTION_H

#include "tasktemplate.h"
#include <pcbinterface.h>

class TaskOffsetGetAdjCorrection : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName, double ourActualValue,
                                  std::shared_ptr<double> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKOFFSETGETADJCORRECTION_H

#ifndef TASKRESETADJDATA_H
#define TASKRESETADJDATA_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskResetAdjData : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
};

#endif // TASKRESETADJDATA_H

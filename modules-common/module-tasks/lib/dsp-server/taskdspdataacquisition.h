#ifndef TASKDSPDATAACQUISITION_H
#define TASKDSPDATAACQUISITION_H

#include <taskservertransactiontemplate.h>
#include <dspinterface.h>

class TaskDspDataAcquisition : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(const Zera::DspInterfacePtr &dspInterface,
                                  DspVarGroupClientInterface* actualValuesDsp, // member of dspInterface
                                  int limitValueCount = 0,
                                  int timeout = TRANSACTION_TIMEOUT, std::function<void()> additionalErrorHandler = []{});
    TaskDspDataAcquisition(const Zera::DspInterfacePtr &dspInterface,
                           DspVarGroupClientInterface* actualValuesDsp,
                           int limitValueCount = 0);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(const QVariant &answer) override;
    Zera::DspInterfacePtr m_dspInterface;
    DspVarGroupClientInterface *m_actualValuesDsp;
    int m_limitValueCount;
};

#endif // TASKDSPDATAACQUISITION_H

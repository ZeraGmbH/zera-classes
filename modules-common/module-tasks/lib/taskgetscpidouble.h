#ifndef TASKGETSCPIDOUBLE_H
#define TASKGETSCPIDOUBLE_H

#include "taskservertransactiontemplate.h"

class TaskGetScpiDouble : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &scpiCmd,
                                  std::shared_ptr<double> result,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskGetScpiDouble(AbstractServerInterfacePtr interface,
                      const QString &scpiCmd,
                      std::shared_ptr<double> result);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    const QString m_scpiCmd;
    const std::shared_ptr<double> m_result;
};

#endif // TASKGETSCPIDOUBLE_H

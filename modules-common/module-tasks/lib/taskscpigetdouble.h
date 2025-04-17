#ifndef TASKSCPIGETDOUBLE_H
#define TASKSCPIGETDOUBLE_H

#include "taskservertransactiontemplate.h"

class TaskScpiGetDouble : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &scpiCmd,
                                  std::shared_ptr<double> result,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskScpiGetDouble(AbstractServerInterfacePtr interface,
                      const QString &scpiCmd,
                      std::shared_ptr<double> result);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    const QString m_scpiCmd;
    const std::shared_ptr<double> m_result;
};

#endif // TASKSCPIGETDOUBLE_H

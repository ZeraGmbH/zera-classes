#ifndef TASKSCPIGETINT_H
#define TASKSCPIGETINT_H

#include "taskservertransactiontemplate.h"

class TaskScpiGetInt : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &scpiCmd,
                                  std::shared_ptr<int> result,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskScpiGetInt(AbstractServerInterfacePtr interface,
                   const QString &scpiCmd,
                   std::shared_ptr<int> result);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    const QString m_scpiCmd;
    const std::shared_ptr<int> m_result;
};

#endif // TASKSCPIGETINT_H

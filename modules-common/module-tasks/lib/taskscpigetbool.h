#ifndef TASKSCPIGETBOOL_H
#define TASKSCPIGETBOOL_H

#include "taskservertransactiontemplate.h"

class TaskScpiGetBool : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &scpiCmd,
                                  std::shared_ptr<bool> result,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskScpiGetBool(AbstractServerInterfacePtr interface,
                    const QString &scpiCmd,
                    std::shared_ptr<bool> result);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    const QString m_scpiCmd;
    const std::shared_ptr<bool> m_result;
};

#endif // TASKSCPIGETBOOL_H

#ifndef TASKSCPIGETSTRING_H
#define TASKSCPIGETSTRING_H

#include <taskservertransactiontemplate.h>

class TaskScpiGetString : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &scpiCmd,
                                  std::shared_ptr<QString> result,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskScpiGetString(AbstractServerInterfacePtr interface,
                      const QString &scpiCmd,
                      std::shared_ptr<QString> result);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    const QString m_scpiCmd;
    const std::shared_ptr<QString> m_result;
};

#endif // TASKSCPIGETSTRING_H

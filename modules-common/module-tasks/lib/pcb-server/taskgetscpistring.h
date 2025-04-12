#ifndef TASKGETSCPISTRING_H
#define TASKGETSCPISTRING_H

#include <taskservertransactiontemplate.h>

class TaskGetScpiString : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &scpiCmd,
                                  std::shared_ptr<QString> result,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskGetScpiString(AbstractServerInterfacePtr interface,
                      const QString &scpiCmd,
                      std::shared_ptr<QString> result);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    const QString m_scpiCmd;
    const std::shared_ptr<QString> m_result;
};

#endif // TASKGETSCPISTRING_H

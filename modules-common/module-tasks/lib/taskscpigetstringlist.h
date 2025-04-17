#ifndef TASKSCPIGETSTRINGLIST_H
#define TASKSCPIGETSTRINGLIST_H

#include "taskservertransactiontemplate.h"

class TaskScpiGetStringList : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &scpiCmd,
                                  std::shared_ptr<QStringList> result,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskScpiGetStringList(AbstractServerInterfacePtr interface,
                          const QString &scpiCmd,
                          std::shared_ptr<QStringList> result);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    const QString m_scpiCmd;
    const std::shared_ptr<QStringList> m_result;
};

#endif // TASKSCPIGETSTRINGLIST_H

#ifndef TASKRESMANIDENT_H
#define TASKRESMANIDENT_H

#include "taskservertransactiontemplate.h"
#include <rminterface.h>

class TaskResmanIdent : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(const Zera::RMInterfacePtr &rmInterface,
                                  const QString &moduleIdent,
                                  int timeout = TRANSACTION_TIMEOUT, std::function<void()> additionalErrorHandler = []{});
    TaskResmanIdent(const Zera::RMInterfacePtr &rmInterface, const QString &moduleIdent);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(const QVariant &answer) override;

    Zera::RMInterfacePtr m_rmInterface;
    const QString m_moduleIdent;
};

#endif // TASKRESMANIDENT_H

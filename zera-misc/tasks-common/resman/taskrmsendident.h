#ifndef TASKRMSENDIDENT_H
#define TASKRMSENDIDENT_H

#include "taskservertransactiontemplate.h"
#include "rminterface.h"

class TaskRmSendIdent : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::RMInterfacePtr rmInterface,
                                   QString ident,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRmSendIdent(Zera::Server::RMInterfacePtr rmInterface,
                    QString ident,
                    int timeout, std::function<void()> additionalErrorHandler = []{});

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::Server::RMInterfacePtr m_rmInterface;
    QString m_ident;
};

#endif // TASKRMSENDIDENT_H

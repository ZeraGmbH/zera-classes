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
    quint32 sendToServer() override;
protected:
    bool handleCheckedServerAnswer(QVariant answer) override;
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    QString m_ident;
};

#endif // TASKRMSENDIDENT_H

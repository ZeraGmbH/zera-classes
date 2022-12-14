#ifndef TASKRMSENDIDENT_H
#define TASKRMSENDIDENT_H

#include "taskcomposit.h"
#include "rminterface.h"

class TaskRmSendIdent : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::RMInterfacePtr rmInterface,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRmSendIdent(Zera::Server::RMInterfacePtr rmInterface);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply);
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    quint32 m_msgnr;
};

#endif // TASKRMSENDIDENT_H

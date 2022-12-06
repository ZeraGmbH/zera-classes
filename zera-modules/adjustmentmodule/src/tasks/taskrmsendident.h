#ifndef TASKRMSENDIDENT_H
#define TASKRMSENDIDENT_H

#include "taskcomposit.h"
#include "rminterface.h"

namespace ADJUSTMENTMODULE {

class TaskRmSendIdent : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskRmSendIdent> create(Zera::Server::RMInterfacePtr rmInterface);
    TaskRmSendIdent(Zera::Server::RMInterfacePtr rmInterface);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply);
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    quint32 m_msgnr;
};
}

#endif // TASKRMSENDIDENT_H

#ifndef TASKRMSENDIDENT_H
#define TASKRMSENDIDENT_H

#include "taskcomposit.h"
#include "rminterface.h"
#include <QVariant>
#include <functional>

namespace ADJUSTMENTMODULE
{
class TaskRmSendIdent : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskRmSendIdent> create(
            Zera::Server::cRMInterface* rmInterface,
            std::function<void(QVariant)> errMsgFunction = [](QVariant){});
    TaskRmSendIdent(Zera::Server::cRMInterface* rmInterface, std::function<void(QVariant)> errMsgFunction);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply);
private:
    Zera::Server::cRMInterface* m_rmInterface;
    std::function<void(QVariant)> m_errMsgFunction;
    quint32 m_msgnr;
};
}

#endif // TASKRMSENDIDENT_H

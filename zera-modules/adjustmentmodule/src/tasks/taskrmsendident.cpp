#include "taskrmsendident.h"
#include "reply.h"
#include "errormessages.h"

namespace ADJUSTMENTMODULE
{

std::unique_ptr<TaskRmSendIdent> TaskRmSendIdent::create(Zera::Server::RMInterfacePtr rmInterface, std::function<void (QVariant)> errMsgFunction)
{
    return std::make_unique<TaskRmSendIdent>(rmInterface, errMsgFunction);
}

TaskRmSendIdent::TaskRmSendIdent(Zera::Server::RMInterfacePtr rmInterface, std::function<void (QVariant)> errMsgFunction) :
    m_rmInterface(rmInterface),
    m_errMsgFunction(errMsgFunction)
{
}

void TaskRmSendIdent::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskRmSendIdent::onRmAnswer);
    m_msgnr = m_rmInterface->rmIdent(QString("Adjustment"));
}

void TaskRmSendIdent::onRmAnswer(quint32 msgnr, quint8 reply)
{
    if(msgnr == m_msgnr) {
        if(reply != ack)
            m_errMsgFunction(rmidentErrMSG);
        finishTask(reply == ack);
    }
}

}

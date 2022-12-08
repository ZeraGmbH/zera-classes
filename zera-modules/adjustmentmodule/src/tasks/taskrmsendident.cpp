#include "taskrmsendident.h"
#include "reply.h"
#include "tasktimeoutdecorator.h"

namespace ADJUSTMENTMODULE
{

std::unique_ptr<TaskComposite> TaskRmSendIdent::create(Zera::Server::RMInterfacePtr rmInterface)
{
    return std::make_unique<TaskRmSendIdent>(rmInterface);
}

std::unique_ptr<TaskComposite> TaskRmSendIdent::create(Zera::Server::RMInterfacePtr rmInterface, int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(rmInterface), additionalErrorHandler);
}

TaskRmSendIdent::TaskRmSendIdent(Zera::Server::RMInterfacePtr rmInterface) :
    m_rmInterface(rmInterface)
{
}

void TaskRmSendIdent::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskRmSendIdent::onRmAnswer);
    m_msgnr = m_rmInterface->rmIdent(QString("Adjustment"));
}

void TaskRmSendIdent::onRmAnswer(quint32 msgnr, quint8 reply)
{
    if(msgnr == m_msgnr)
        finishTask(reply == ack);
}

}

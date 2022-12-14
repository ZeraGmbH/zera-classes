#include "taskrmsendident.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

TaskCompositePtr TaskRmSendIdent::create(Zera::Server::RMInterfacePtr rmInterface,
                                         int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskRmSendIdent>(rmInterface),
                                             additionalErrorHandler);
}

TaskRmSendIdent::TaskRmSendIdent(Zera::Server::RMInterfacePtr rmInterface) :
    m_rmInterface(rmInterface)
{
}

void TaskRmSendIdent::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskRmSendIdent::onServerAnswer);
    m_msgnr = m_rmInterface->rmIdent(QString("Adjustment"));
}

void TaskRmSendIdent::onServerAnswer(quint32 msgnr, quint8 reply)
{
    if(msgnr == m_msgnr)
        finishTask(reply == ack);
}

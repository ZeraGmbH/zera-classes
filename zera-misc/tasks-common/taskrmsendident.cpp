#include "taskrmsendident.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

TaskCompositePtr TaskRmSendIdent::create(Zera::Server::RMInterfacePtr rmInterface, QString ident,
                                         int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskRmSendIdent>(
                                                 rmInterface,
                                                 ident),
                                             additionalErrorHandler);
}

TaskRmSendIdent::TaskRmSendIdent(Zera::Server::RMInterfacePtr rmInterface, QString ident) :
    m_rmInterface(rmInterface),
    m_ident(ident)
{
}

void TaskRmSendIdent::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskRmSendIdent::onServerAnswer);
    m_msgnr = m_rmInterface->rmIdent(m_ident);
}

void TaskRmSendIdent::onServerAnswer(quint32 msgnr, quint8 reply)
{
    if(msgnr == m_msgnr)
        finishTask(reply == ack);
}

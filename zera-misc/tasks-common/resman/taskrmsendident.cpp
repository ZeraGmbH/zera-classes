#include "taskrmsendident.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

TaskCompositePtr TaskRmSendIdent::create(Zera::Server::RMInterfacePtr rmInterface,
                                         QString ident,
                                         int timeout, std::function<void ()> additionalErrorHandler)
{
    return std::make_unique<TaskRmSendIdent>(
                rmInterface,
                ident,
                timeout, additionalErrorHandler);
}

TaskRmSendIdent::TaskRmSendIdent(Zera::Server::RMInterfacePtr rmInterface,
                                 QString ident,
                                 int timeout, std::function<void ()> additionalErrorHandler) :
    TaskServerTransactionTemplate(rmInterface),
    m_rmInterface(rmInterface),
    m_ident(ident)
{
}

quint32 TaskRmSendIdent::sendToServer()
{
    return m_rmInterface->rmIdent(m_ident);
}

bool TaskRmSendIdent::handleCheckedServerAnswer(QVariant answer)
{
    return true;
}

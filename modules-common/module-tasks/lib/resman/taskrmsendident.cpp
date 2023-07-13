#include "taskrmsendident.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRmSendIdent::create(Zera::RMInterfacePtr rmInterface,
                                         QString ident,
                                         int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRmSendIdent>(
                                                 rmInterface,
                                                 ident),
                                             additionalErrorHandler);
}

TaskRmSendIdent::TaskRmSendIdent(Zera::RMInterfacePtr rmInterface,
                                 QString ident) :
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

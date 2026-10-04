#include "taskresmanident.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskResmanIdent::create(const Zera::RMInterfacePtr &rmInterface,
                                        const QString &moduleIdent,
                                        int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskResmanIdent>(rmInterface, moduleIdent),
                                             additionalErrorHandler);
}

TaskResmanIdent::TaskResmanIdent(const Zera::RMInterfacePtr &rmInterface, const QString &moduleIdent) :
    TaskServerTransactionTemplate(rmInterface),
    m_rmInterface(rmInterface),
    m_moduleIdent(moduleIdent)
{
}

quint32 TaskResmanIdent::sendToServer()
{
    return m_rmInterface->rmIdent(m_moduleIdent);
}

bool TaskResmanIdent::handleCheckedServerAnswer(const QVariant &answer)
{
    Q_UNUSED(answer);
    return true;
}

#include "taskscpigetstringlist.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskScpiGetStringList::create(const AbstractServerInterfacePtr &interface,
                                              const QString &scpiCmd,
                                              std::shared_ptr<QStringList> result, const QString &separator,
                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskScpiGetStringList>(
                                                 interface,
                                                 scpiCmd,
                                                 result,
                                                 separator),
                                             additionalErrorHandler);
}

TaskScpiGetStringList::TaskScpiGetStringList(const AbstractServerInterfacePtr &interface,
                                             const QString &scpiCmd,
                                             std::shared_ptr<QStringList> result, const QString &separator) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result),
    m_separator(separator)
{
}

quint32 TaskScpiGetStringList::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskScpiGetStringList::handleCheckedServerAnswer(const QVariant &answer)
{
    *m_result = answer.toString().split(m_separator);
    return true;
}

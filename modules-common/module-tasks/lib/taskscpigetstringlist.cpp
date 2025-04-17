#include "taskscpigetstringlist.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskScpiGetStringList::create(AbstractServerInterfacePtr interface,
                                              const QString &scpiCmd,
                                              std::shared_ptr<QStringList> result,
                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskScpiGetStringList>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);
}

TaskScpiGetStringList::TaskScpiGetStringList(AbstractServerInterfacePtr interface,
                                             const QString &scpiCmd,
                                             std::shared_ptr<QStringList> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskScpiGetStringList::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskScpiGetStringList::handleCheckedServerAnswer(QVariant answer)
{
    *m_result = answer.toString().split(";");
    return true;
}

#include "taskgetscpistringlist.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskGetScpiStringList::create(AbstractServerInterfacePtr interface,
                                              const QString &scpiCmd,
                                              std::shared_ptr<QStringList> result,
                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskGetScpiStringList>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);
}

TaskGetScpiStringList::TaskGetScpiStringList(AbstractServerInterfacePtr interface,
                                             const QString &scpiCmd,
                                             std::shared_ptr<QStringList> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskGetScpiStringList::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);

}

bool TaskGetScpiStringList::handleCheckedServerAnswer(QVariant answer)
{
    *m_result = answer.toString().split(";");
    return true;
}

#include "taskgetscpistring.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskGetScpiString::create(AbstractServerInterfacePtr interface,
                                          const QString &scpiCmd,
                                          std::shared_ptr<QString> result,
                                          int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskGetScpiString>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);

}

TaskGetScpiString::TaskGetScpiString(AbstractServerInterfacePtr interface,
                                     const QString &scpiCmd,
                                     std::shared_ptr<QString> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskGetScpiString::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskGetScpiString::handleCheckedServerAnswer(QVariant answer)
{
    *m_result = answer.toString();
    return true;
}

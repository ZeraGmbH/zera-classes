#include "taskscpigetstring.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskScpiGetString::create(AbstractServerInterfacePtr interface,
                                          const QString &scpiCmd,
                                          std::shared_ptr<QString> result,
                                          int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskScpiGetString>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);

}

TaskScpiGetString::TaskScpiGetString(AbstractServerInterfacePtr interface,
                                     const QString &scpiCmd,
                                     std::shared_ptr<QString> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskScpiGetString::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskScpiGetString::handleCheckedServerAnswer(QVariant answer)
{
    *m_result = answer.toString();
    return true;
}

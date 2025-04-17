#include "taskscpigetint.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskScpiGetInt::create(AbstractServerInterfacePtr interface,
                                       const QString &scpiCmd,
                                       std::shared_ptr<int> result,
                                       int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskScpiGetInt>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);
}

TaskScpiGetInt::TaskScpiGetInt(AbstractServerInterfacePtr interface,
                               const QString &scpiCmd,
                               std::shared_ptr<int> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskScpiGetInt::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskScpiGetInt::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_result = answer.toInt(&ok);
    return ok;
}

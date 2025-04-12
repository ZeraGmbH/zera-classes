#include "taskgetscpiint.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskGetScpiInt::create(AbstractServerInterfacePtr interface,
                                       const QString &scpiCmd,
                                       std::shared_ptr<int> result,
                                       int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskGetScpiInt>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);
}

TaskGetScpiInt::TaskGetScpiInt(AbstractServerInterfacePtr interface,
                               const QString &scpiCmd,
                               std::shared_ptr<int> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskGetScpiInt::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskGetScpiInt::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_result = answer.toInt(&ok);
    return ok;
}

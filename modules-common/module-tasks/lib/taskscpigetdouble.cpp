#include "taskscpigetdouble.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskScpiGetDouble::create(AbstractServerInterfacePtr interface, const QString &scpiCmd, std::shared_ptr<double> result, int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskScpiGetDouble>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);
}

TaskScpiGetDouble::TaskScpiGetDouble(AbstractServerInterfacePtr interface,
                                     const QString &scpiCmd,
                                     std::shared_ptr<double> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskScpiGetDouble::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskScpiGetDouble::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_result = answer.toDouble(&ok);
    return ok;
}

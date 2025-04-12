#include "taskgetscpidouble.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskGetScpiDouble::create(AbstractServerInterfacePtr interface, const QString &scpiCmd, std::shared_ptr<double> result, int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskGetScpiDouble>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);
}

TaskGetScpiDouble::TaskGetScpiDouble(AbstractServerInterfacePtr interface,
                                     const QString &scpiCmd,
                                     std::shared_ptr<double> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskGetScpiDouble::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskGetScpiDouble::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_result = answer.toDouble(&ok);
    return ok;
}

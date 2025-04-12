#include "taskgetscpibool.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskGetScpiBool::create(AbstractServerInterfacePtr interface,
                                        const QString &scpiCmd,
                                        std::shared_ptr<bool> result,
                                        int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskGetScpiBool>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);
}

TaskGetScpiBool::TaskGetScpiBool(AbstractServerInterfacePtr interface,
                                 const QString &scpiCmd,
                                 std::shared_ptr<bool> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskGetScpiBool::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskGetScpiBool::handleCheckedServerAnswer(QVariant answer)
{
    *m_result = answer.toBool();
    return true;
}

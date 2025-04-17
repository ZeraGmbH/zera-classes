#include "taskscpigetbool.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskScpiGetBool::create(AbstractServerInterfacePtr interface,
                                        const QString &scpiCmd,
                                        std::shared_ptr<bool> result,
                                        int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskScpiGetBool>(
                                                 interface,
                                                 scpiCmd,
                                                 result),
                                             additionalErrorHandler);
}

TaskScpiGetBool::TaskScpiGetBool(AbstractServerInterfacePtr interface,
                                 const QString &scpiCmd,
                                 std::shared_ptr<bool> result) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd),
    m_result(result)
{
}

quint32 TaskScpiGetBool::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskScpiGetBool::handleCheckedServerAnswer(QVariant answer)
{
    *m_result = answer.toBool();
    return true;
}

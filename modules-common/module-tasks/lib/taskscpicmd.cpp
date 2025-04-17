#include "taskscpicmd.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskScpiCmd::create(AbstractServerInterfacePtr interface,
                                    const QString &scpiCmd,
                                    int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskScpiCmd>(
                                                 interface,
                                                 scpiCmd),
                                             additionalErrorHandler);
}

TaskScpiCmd::TaskScpiCmd(AbstractServerInterfacePtr interface, const QString &scpiCmd) :
    TaskServerTransactionTemplate(interface),
    m_scpiCmd(scpiCmd)
{
}

quint32 TaskScpiCmd::sendToServer()
{
    return m_interface->scpiCommand(m_scpiCmd);
}

bool TaskScpiCmd::handleCheckedServerAnswer(QVariant answer)
{
    Q_UNUSED(answer)
    return true;
}

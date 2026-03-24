#include "taskhasinternalsourcegenerator.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskHasInternalSourceGenerator::create(AbstractServerInterfacePtr interface,
                                                       std::shared_ptr<bool> hasInternalSourceGenerator,
                                                       int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskHasInternalSourceGenerator>(interface, hasInternalSourceGenerator),
                                             additionalErrorHandler);
}

TaskHasInternalSourceGenerator::TaskHasInternalSourceGenerator(AbstractServerInterfacePtr interface,
                                                               std::shared_ptr<bool> hasInternalSourceGenerator) :
    m_interface(interface),
    m_hasInternalSourceGenerator(hasInternalSourceGenerator)
{
}

// Accept all!!
void TaskHasInternalSourceGenerator::start()
{
    connect(m_interface.get(), &AbstractServerInterface::serverAnswer,
            this, &TaskHasInternalSourceGenerator::onServerAnswer);
    connect(m_interface.get(), &AbstractServerInterface::tcpError, [&]() {
        finishTask(false);
    });
    m_msgnr = m_interface->scpiCommand("STATUS:HASSOURCEGENERATOR?");
}

void TaskHasInternalSourceGenerator::onServerAnswer(quint32 msgnr, quint8 reply, const QVariant &answer)
{
    Q_UNUSED(reply)
    if (m_msgnr == msgnr) {
        *m_hasInternalSourceGenerator = answer == "1";
        finishTask(true);
    }
}

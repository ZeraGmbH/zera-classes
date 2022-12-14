#include "taskrmcheckresourcetype.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

TaskCompositePtr TaskRmCheckResourceType::create(Zera::Server::RMInterfacePtr rmInterface,
                                                 int timeout, std::function<void ()> additionalErrorHandler,
                                                 QString checkResourceType)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskRmCheckResourceType>(
                                                 rmInterface, checkResourceType),
                                             additionalErrorHandler);
}

TaskRmCheckResourceType::TaskRmCheckResourceType(Zera::Server::RMInterfacePtr rmInterface, QString checkResourceType) :
    m_rmInterface(rmInterface),
    m_checkResourceType(checkResourceType)
{
}

void TaskRmCheckResourceType::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskRmCheckResourceType::onServerAnswer);
    m_msgnr = m_rmInterface->getResourceTypes();
}

void TaskRmCheckResourceType::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(msgnr == m_msgnr) {
        if((reply == ack) && (answer.toString().contains(m_checkResourceType)))
            finishTask(true);
        else
            finishTask(false);
    }
}

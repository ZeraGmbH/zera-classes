#include "taskrmcheckresourcetype.h"
#include "reply.h"

std::unique_ptr<TaskRmCheckResourceType> TaskRmCheckResourceType::create(Zera::Server::RMInterfacePtr rmInterface, QString checkResourceType)
{
    return std::make_unique<TaskRmCheckResourceType>(rmInterface, checkResourceType);
}

TaskRmCheckResourceType::TaskRmCheckResourceType(Zera::Server::RMInterfacePtr rmInterface, QString checkResourceType) :
    m_rmInterface(rmInterface),
    m_checkResourceType(checkResourceType)
{
}

void TaskRmCheckResourceType::start()
{
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &TaskRmCheckResourceType::onRmAnswer);
    m_msgnr = m_rmInterface->getResourceTypes();
}

void TaskRmCheckResourceType::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(msgnr == m_msgnr) {
        if ((reply == ack) && (answer.toString().contains(m_checkResourceType)))
            finishTask(true);
        else
            finishTask(false);
    }
}

#include "taskrmcheckresourcetype.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRmCheckResourceType::create(Zera::RMInterfacePtr rmInterface,
                                                 int timeout, std::function<void ()> additionalErrorHandler,
                                                 QString checkResourceType)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRmCheckResourceType>(
                                                 rmInterface, checkResourceType),
                                             additionalErrorHandler);
}

TaskRmCheckResourceType::TaskRmCheckResourceType(Zera::RMInterfacePtr rmInterface, QString checkResourceType) :
    TaskServerTransactionTemplate(rmInterface),
    m_rmInterface(rmInterface),
    m_checkResourceType(checkResourceType.toUpper())
{
}

quint32 TaskRmCheckResourceType::sendToServer()
{
    return m_rmInterface->getResourceTypes();
}

bool TaskRmCheckResourceType::handleCheckedServerAnswer(QVariant answer)
{
    return answer.toString().toUpper().contains(m_checkResourceType);
}

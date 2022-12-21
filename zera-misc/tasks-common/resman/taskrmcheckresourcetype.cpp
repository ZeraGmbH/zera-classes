#include "taskrmcheckresourcetype.h"
#include "tasktimeoutdecorator.h"

TaskCompositePtr TaskRmCheckResourceType::create(AbstractRmInterfacePtr rmInterface,
                                                 int timeout, std::function<void ()> additionalErrorHandler,
                                                 QString checkResourceType)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskRmCheckResourceType>(
                                                 rmInterface, checkResourceType),
                                             additionalErrorHandler);
}

TaskRmCheckResourceType::TaskRmCheckResourceType(AbstractRmInterfacePtr rmInterface, QString checkResourceType) :
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

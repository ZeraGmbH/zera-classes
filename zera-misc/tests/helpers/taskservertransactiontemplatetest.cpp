#include "taskservertransactiontemplatetest.h"

TaskCompositePtr TaskServerTransactionTemplateTest::create(AbstractRmInterfacePtr server, QString &receivedAnswer)
{
    return std::make_unique<TaskServerTransactionTemplateTest>(server, receivedAnswer);
}

TaskServerTransactionTemplateTest::TaskServerTransactionTemplateTest(AbstractRmInterfacePtr server, QString &receivedAnswer) :
    TaskServerTransactionTemplate(server),
    m_rmserver(server),
    m_receivedAnswer(receivedAnswer)
{
}

quint32 TaskServerTransactionTemplateTest::sendToServer()
{
    return m_rmserver->getResources("");
}

bool TaskServerTransactionTemplateTest::handleServerAnswer(QVariant answer)
{
    m_receivedAnswer = answer.toString();
    return true;
}

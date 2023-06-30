#include "taskservertransactiontemplatetest.h"

TaskTemplatePtr TaskServerTransactionTemplateTest::create(Zera::RMInterfacePtr server, QString &receivedAnswer)
{
    return std::make_unique<TaskServerTransactionTemplateTest>(server, receivedAnswer);
}

TaskServerTransactionTemplateTest::TaskServerTransactionTemplateTest(Zera::RMInterfacePtr server, QString &receivedAnswer) :
    TaskServerTransactionTemplate(server),
    m_rmserver(server),
    m_receivedAnswer(receivedAnswer)
{
}

quint32 TaskServerTransactionTemplateTest::sendToServer()
{
    return m_rmserver->getResources("");
}

bool TaskServerTransactionTemplateTest::handleCheckedServerAnswer(QVariant answer)
{
    m_receivedAnswer = answer.toString();
    return true;
}

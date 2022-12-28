#ifndef TASKSERVERTRANSACTIONTEMPLATETEST_H
#define TASKSERVERTRANSACTIONTEMPLATETEST_H

#include "taskservertransactiontemplate.h"
#include <rminterface.h>

class TaskServerTransactionTemplateTest : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::RMInterfacePtr server, QString &receivedAnswer);
    TaskServerTransactionTemplateTest(Zera::Server::RMInterfacePtr server, QString &receivedAnswer);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::Server::RMInterfacePtr m_rmserver;
    QString &m_receivedAnswer;
};

typedef std::unique_ptr<TaskServerTransactionTemplateTest> TaskServerTransactionTemplateTestPtr;

#endif // TASKSERVERTRANSACTIONTEMPLATETEST_H

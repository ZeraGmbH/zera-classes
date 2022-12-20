#ifndef TASKSERVERTRANSACTIONTEMPLATETEST_H
#define TASKSERVERTRANSACTIONTEMPLATETEST_H

#include "taskservertransactiontemplate.h"
#include "abstractrminterface.h"

class TaskServerTransactionTemplateTest : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskCompositePtr create(AbstractRmInterfacePtr server, QString &receivedAnswer);
    TaskServerTransactionTemplateTest(AbstractRmInterfacePtr server, QString &receivedAnswer);
    virtual quint32 sendToServer();
protected:
    virtual bool handleServerAnswer(QVariant answer);
private:
    AbstractRmInterfacePtr m_rmserver;
    QString &m_receivedAnswer;
};

typedef std::unique_ptr<TaskServerTransactionTemplateTest> TaskServerTransactionTemplateTestPtr;

#endif // TASKSERVERTRANSACTIONTEMPLATETEST_H

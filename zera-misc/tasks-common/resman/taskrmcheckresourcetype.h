#ifndef TASKRMCHECKRESOURCETYPE_H
#define TASKRMCHECKRESOURCETYPE_H

#include "taskservertransactiontemplate.h"
#include "abstractrminterface.h"

class TaskRmCheckResourceType : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskCompositePtr create(AbstractRmInterfacePtr rmInterface,
                                   int timeout,
                                   std::function<void()> additionalErrorHandler = []{},
                                   QString checkResourceType = "SENSE");
    TaskRmCheckResourceType(AbstractRmInterfacePtr rmInterface, QString checkResourceType);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    AbstractRmInterfacePtr m_rmInterface;
    QString m_checkResourceType;
    quint32 m_msgnr;
};

#endif // TASKRMCHECKRESOURCETYPE_H

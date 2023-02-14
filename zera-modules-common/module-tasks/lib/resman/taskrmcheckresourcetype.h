#ifndef TASKRMCHECKRESOURCETYPE_H
#define TASKRMCHECKRESOURCETYPE_H

#include "taskservertransactiontemplate.h"
#include <rminterface.h>

class TaskRmCheckResourceType : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::RMInterfacePtr rmInterface,
                                   int timeout,
                                   std::function<void()> additionalErrorHandler = []{},
                                   QString checkResourceType = "SENSE");
    TaskRmCheckResourceType(Zera::RMInterfacePtr rmInterface, QString checkResourceType);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::RMInterfacePtr m_rmInterface;
    QString m_checkResourceType;
};

#endif // TASKRMCHECKRESOURCETYPE_H

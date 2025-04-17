#ifndef TASKSCPICMD_H
#define TASKSCPICMD_H

#include "taskservertransactiontemplate.h"

class TaskScpiCmd : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  const QString &scpiCmd,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskScpiCmd(AbstractServerInterfacePtr interface,
                const QString &scpiCmd);

private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    const QString m_scpiCmd;
};

#endif // TASKSCPICMD_H

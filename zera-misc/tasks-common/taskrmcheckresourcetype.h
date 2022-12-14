#ifndef TASKRMCHECKRESOURCETYPE_H
#define TASKRMCHECKRESOURCETYPE_H

#include "taskcomposit.h"
#include "rminterface.h"
#include <QString>

class TaskRmCheckResourceType : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::RMInterfacePtr rmInterface,
                                   int timeout,
                                   std::function<void()> additionalErrorHandler = []{},
                                   QString checkResourceType = "SENSE");
    TaskRmCheckResourceType(Zera::Server::RMInterfacePtr rmInterface, QString checkResourceType);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    QString m_checkResourceType;
    quint32 m_msgnr;
};

#endif // TASKRMCHECKRESOURCETYPE_H

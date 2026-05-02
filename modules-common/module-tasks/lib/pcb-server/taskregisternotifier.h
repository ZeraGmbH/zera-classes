#ifndef TASKREGISTERNOTIFIER_H
#define TASKREGISTERNOTIFIER_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskRegisterNotifier : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface,
                                  const QString &scpiQuery,
                                  int notificationId,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRegisterNotifier(const Zera::PcbInterfacePtr &pcbInterface,
                         const QString &scpiQuery,
                         int notificationId);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(const QVariant &answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_scpiQuery;
    int m_notificationId;
};

#endif // TASKREGISTERNOTIFIER_H

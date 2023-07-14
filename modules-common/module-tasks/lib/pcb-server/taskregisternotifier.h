#ifndef TASKREGISTERNOTIFIER_H
#define TASKREGISTERNOTIFIER_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskRegisterNotifier : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString scpiQuery,
                                  int notificationId,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRegisterNotifier(Zera::PcbInterfacePtr pcbInterface, QString scpiQuery, int notificationId);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_scpiQuery;
    int m_notificationId;
};

#endif // TASKREGISTERNOTIFIER_H

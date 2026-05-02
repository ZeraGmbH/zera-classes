#ifndef TASKREGISTERNOTIFIERWITHVALUE_H
#define TASKREGISTERNOTIFIERWITHVALUE_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskRegisterNotifierWithValue : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface,
                                  const QString &scpiQuery,
                                  int notificationId,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRegisterNotifierWithValue(const Zera::PcbInterfacePtr &pcbInterface,
                                  const QString &scpiQuery,
                                  int notificationId);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(const QVariant &answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_scpiQuery;
    int m_notificationId;
};

#endif // TASKREGISTERNOTIFIERWITHVALUE_H

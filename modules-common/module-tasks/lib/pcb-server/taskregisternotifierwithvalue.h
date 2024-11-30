#ifndef TASKREGISTERNOTIFIERWITHVALUE_H
#define TASKREGISTERNOTIFIERWITHVALUE_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskRegisterNotifierWithValue : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString scpiQuery,
                                  int notificationId,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRegisterNotifierWithValue(Zera::PcbInterfacePtr pcbInterface, QString scpiQuery, int notificationId);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_scpiQuery;
    int m_notificationId;
};

#endif // TASKREGISTERNOTIFIERWITHVALUE_H

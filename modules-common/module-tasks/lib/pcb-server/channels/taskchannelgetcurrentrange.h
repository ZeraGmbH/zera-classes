#ifndef TASKCHANNELGETCURRENTRANGE_H
#define TASKCHANNELGETCURRENTRANGE_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskChannelGetCurrentRange : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface, const QString &channelName,
                                  QString& valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetCurrentRange(const Zera::PcbInterfacePtr &pcbInterface,
                               const QString &channelName,
                               QString& valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(const QVariant &answer) override;

    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    QString& m_valueReceived;
};

#endif // TASKCHANNELGETCURRENTRANGE_H

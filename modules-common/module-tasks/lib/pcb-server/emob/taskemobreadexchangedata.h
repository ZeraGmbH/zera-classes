#ifndef TASKEMOBREADEXCHANGEDATA_H
#define TASKEMOBREADEXCHANGEDATA_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobReadExchangeData : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  std::shared_ptr<QByteArray> exchangeDataReceived,
                                  const QString &channelMName,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
    TaskEmobReadExchangeData(Zera::PcbInterfacePtr pcbInterface,
                             std::shared_ptr<QByteArray> exchangeDataReceived,
                             const QString &channelMName);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(const QVariant &answer) override;
    std::shared_ptr<QByteArray> m_exchangeDataReceived;
    QString m_channelMName;
};

#endif // TASKEMOBREADEXCHANGEDATA_H

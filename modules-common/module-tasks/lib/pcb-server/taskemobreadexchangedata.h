#ifndef TASKEMOBREADEXCHANGEDATA_H
#define TASKEMOBREADEXCHANGEDATA_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobReadExchangeData : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  std::shared_ptr<QByteArray> exchangeDataReceived,
                                  QString channelMName,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
    TaskEmobReadExchangeData(Zera::PcbInterfacePtr pcbInterface,
                             std::shared_ptr<QByteArray> exchangeDataReceived,
                             QString channelMName);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    const std::shared_ptr<int> m_result;
    std::shared_ptr<QByteArray> m_exchangeDataReceived;
    QString m_channelMName;
};

#endif // TASKEMOBREADEXCHANGEDATA_H

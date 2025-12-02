#ifndef TASKEMOBWRITEEXCHANGEDATA_H
#define TASKEMOBWRITEEXCHANGEDATA_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobWriteExchangeData : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, int emobIdFrom0To3,
                                  std::shared_ptr<QByteArray> exchangeDataWrite,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
    TaskEmobWriteExchangeData(Zera::PcbInterfacePtr pcbInterface,
                              QString channelMName, int emobIdFrom0To3,
                              std::shared_ptr<QByteArray> exchangeDataWrite);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    QString m_channelMName;
    int m_emobIdFrom0To3;
    std::shared_ptr<QByteArray> m_exchangeDataWrite;
};

#endif // TASKEMOBWRITEEXCHANGEDATA_H

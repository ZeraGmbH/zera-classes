#ifndef TASKEMOBWRITEEXCHANGEDATA_H
#define TASKEMOBWRITEEXCHANGEDATA_H

#include <pcbinterface.h>
#include <taskservertransactiontemplate.h>

class TaskEmobWriteExchangeData : public TaskServerTransactionTemplate
{
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QByteArray exchangeDataWrite,
                                  QString channelMName,
                                  int timeout,
                                  std::function<void()> additionalErrorHandler = []{});
    TaskEmobWriteExchangeData(Zera::PcbInterfacePtr pcbInterface,
                              QByteArray exchangeDataWrite,
                              QString channelMName);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    QByteArray m_exchangeDataWrite;
    QString m_channelMName;
};

#endif // TASKEMOBWRITEEXCHANGEDATA_H

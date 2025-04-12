#ifndef TASKCHANNELGETDSPCHANNEL_H
#define TASKCHANNELGETDSPCHANNEL_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskChannelGetDspChannel : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface, QString channelName,
                                  std::shared_ptr<int> valueReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetDspChannel(Zera::PcbInterfacePtr pcbInterface,
                             QString channelName,
                             std::shared_ptr<int> valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    std::shared_ptr<int> m_valueReceived;
};

#endif // TASKCHANNELGETDSPCHANNEL_H

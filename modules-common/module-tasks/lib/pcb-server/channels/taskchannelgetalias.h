#ifndef TASKCHANNELGETALIAS_H
#define TASKCHANNELGETALIAS_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskChannelGetAlias : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface, QString channelName,
                                   QString& valueReceived,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetAlias(Zera::PcbInterfacePtr pcbInterface,
                         QString channelName,
                         QString& valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    QString& m_valueReceived;
};

#endif // TASKCHANNELGETALIAS_H

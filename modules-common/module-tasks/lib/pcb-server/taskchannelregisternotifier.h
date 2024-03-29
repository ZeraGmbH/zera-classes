#ifndef TASKCHANNELREGISTERNOTIFIER_H
#define TASKCHANNELREGISTERNOTIFIER_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskChannelRegisterNotifier : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                   QString channelName,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelRegisterNotifier(Zera::PcbInterfacePtr pcbInterface, QString channelName);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
};

#endif // TASKCHANNELREGISTERNOTIFIER_H

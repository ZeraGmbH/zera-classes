#ifndef TASKCHANNELREGISTERNOTIFIER_H
#define TASKCHANNELREGISTERNOTIFIER_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskChannelRegisterNotifier : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::Server::PcbInterfacePtr pcbInterface,
                                   QString channelName,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelRegisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    quint32 m_msgnr;
};

#endif // TASKCHANNELREGISTERNOTIFIER_H

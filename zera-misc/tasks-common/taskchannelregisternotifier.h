#ifndef TASKCHANNELREGISTERNOTIFIER_H
#define TASKCHANNELREGISTERNOTIFIER_H

#include "taskcomposit.h"
#include "pcbinterface.h"

class TaskChannelRegisterNotifier : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName);
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelRegisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    quint32 m_msgnr;
};

#endif // TASKCHANNELREGISTERNOTIFIER_H

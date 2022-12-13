#ifndef TASKCHANNELUNREGISTERNOTIFIER_H
#define TASKCHANNELUNREGISTERNOTIFIER_H

#include "taskcomposit.h"
#include "pcbinterface.h"

class TaskChannelUnregisterNotifier : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName);
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelUnregisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    quint32 m_msgnr;
};

#endif // TASKCHANNELUNREGISTERNOTIFIER_H

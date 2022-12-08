#ifndef TASKREADCHANNELIPPORT_H
#define TASKREADCHANNELIPPORT_H

#include "taskcomposit.h"
#include "rminterface.h"

class TaskReadChannelIpPort : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(Zera::Server::RMInterfacePtr rmInterface, QString channelName, QHash<QString,int> &channelPortHash);
    static std::unique_ptr<TaskComposite> create(Zera::Server::RMInterfacePtr rmInterface, QString channelName, QHash<QString,int> &channelPortHash,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskReadChannelIpPort(Zera::Server::RMInterfacePtr rmInterface, QString channelName, QHash<QString,int> &channelPortHash);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    QString m_channelName;
    QHash<QString,int> &m_channelPortHash;
    quint32 m_msgnr;
};

#endif // TASKREADCHANNELIPPORT_H

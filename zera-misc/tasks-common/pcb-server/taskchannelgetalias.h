#ifndef TASKCHANNELGETALIAS_H
#define TASKCHANNELGETALIAS_H

#include "pcbinterface.h"
#include "taskcomposit.h"

class TaskChannelGetAlias : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                   QString& valueReceived,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetAlias(Zera::Server::PcbInterfacePtr pcbInterface,
                         QString channelName,
                         QString& valueReceived);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    QString& m_valueReceived;
    quint32 m_msgnr;
};

#endif // TASKCHANNELGETALIAS_H

#ifndef TASKCHANNELREADALIAS_H
#define TASKCHANNELREADALIAS_H

#include "pcbinterface.h"
#include "taskcomposit.h"

class TaskChannelReadAlias : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                   QString& valueReceived,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelReadAlias(Zera::Server::PcbInterfacePtr pcbInterface,
                         QString channelName,
                         QString& valueReceived);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    QString& m_valueReceived;
    quint32 m_msgnr;
};

#endif // TASKCHANNELREADALIAS_H

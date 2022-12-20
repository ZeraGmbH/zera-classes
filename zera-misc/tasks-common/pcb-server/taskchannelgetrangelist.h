#ifndef TASKCHANNELREADRANGES_H
#define TASKCHANNELREADRANGES_H

#include "taskcomposit.h"
#include "pcbinterface.h"

class TaskChannelGetRangeList : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::PcbInterfacePtr pcbInterface,
                                   QString channelName,
                                   QStringList &targetRangeList,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetRangeList(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName, QStringList &targetRangeList);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    QStringList &m_targetRangeList;
    quint32 m_msgnr;
};

#endif // TASKCHANNELREADRANGES_H

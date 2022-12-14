#ifndef TASKCHANNELGETURVALUE_H
#define TASKCHANNELGETURVALUE_H

#include "taskcomposit.h"
#include "pcbinterface.h"

class TaskChannelGetUrValue : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName,
                                                 double &urValue);
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName,
                                                 double &urValue,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetUrValue(Zera::Server::PcbInterfacePtr pcbInterface,
                          QString channelSysName, QString rangeName, double &urValue);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelSysName;
    QString m_rangeName;
    double &m_urValue;
    quint32 m_msgnr;
};

#endif // TASKCHANNELGETURVALUE_H

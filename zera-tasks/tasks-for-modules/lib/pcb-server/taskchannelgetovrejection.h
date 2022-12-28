#ifndef TASKCHANNELGETOVREJECTION_H
#define TASKCHANNELGETOVREJECTION_H

#include "taskcomposit.h"
#include <pcbinterface.h>

class TaskChannelGetOvRejection : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::PcbInterfacePtr pcbInterface,
                                   QString channelSysName, QString rangeName,
                                   double &valueReceived,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetOvRejection(Zera::Server::PcbInterfacePtr pcbInterface,
                              QString channelSysName, QString rangeName,
                              double &valueReceived);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelSysName;
    QString m_rangeName;
    double &m_valueReceived;
    quint32 m_msgnr;
};

#endif // TASKCHANNELGETOVREJECTION_H

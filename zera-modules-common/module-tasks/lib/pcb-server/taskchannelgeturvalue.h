#ifndef TASKCHANNELGETURVALUE_H
#define TASKCHANNELGETURVALUE_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskChannelGetUrValue : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::Server::PcbInterfacePtr pcbInterface,
                                   QString channelSysName, QString rangeName,
                                   double &valueReceived,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetUrValue(Zera::Server::PcbInterfacePtr pcbInterface,
                          QString channelSysName, QString rangeName,
                          double &valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelSysName;
    QString m_rangeName;
    double &m_valueReceived;
    quint32 m_msgnr;
};

#endif // TASKCHANNELGETURVALUE_H

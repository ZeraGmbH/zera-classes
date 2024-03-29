#ifndef TASKCHANNELGETREJECTION_H
#define TASKCHANNELGETREJECTION_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskChannelGetRejection : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                   QString channelSysName, QString rangeName,
                                   double &valueReceived,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetRejection(Zera::PcbInterfacePtr pcbInterface,
                            QString channelSysName, QString rangeName,
                            double &valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelSysName;
    QString m_rangeName;
    double &m_valueReceived;
};

#endif // TASKCHANNELGETREJECTION_H

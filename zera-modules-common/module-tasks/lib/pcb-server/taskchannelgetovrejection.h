#ifndef TASKCHANNELGETOVREJECTION_H
#define TASKCHANNELGETOVREJECTION_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskChannelGetOvRejection : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                   QString channelSysName, QString rangeName,
                                   double &valueReceived,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetOvRejection(Zera::PcbInterfacePtr pcbInterface,
                              QString channelSysName, QString rangeName,
                              double &valueReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelSysName;
    QString m_rangeName;
    double &m_valueReceived;
    quint32 m_msgnr;
};

#endif // TASKCHANNELGETOVREJECTION_H

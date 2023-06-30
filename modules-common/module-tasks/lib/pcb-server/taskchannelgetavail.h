#ifndef TASKCHANNELGETAVAIL_H
#define TASKCHANNELGETAVAIL_H

#include <taskservertransactiontemplate.h>
#include <pcbinterface.h>

class TaskChannelGetAvail : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QStringList& channelsReceived,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskChannelGetAvail(Zera::PcbInterfacePtr pcbInterface,
                        QStringList& channelsReceived);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
    QStringList& m_channelsReceived;
};

#endif // TASKCHANNELGETAVAIL_H

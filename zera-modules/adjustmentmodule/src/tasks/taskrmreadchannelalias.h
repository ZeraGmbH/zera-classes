#ifndef TASKRMREADCHANNELALIAS_H
#define TASKRMREADCHANNELALIAS_H

#include "pcbinterface.h"
#include "taskcomposit.h"

class TaskRmReadChannelAlias : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                 QString& targetAlias);
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                 QString& targetAlias,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRmReadChannelAlias(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName, QString& targetAlias);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelName;
    QString& m_targetAlias;
    quint32 m_msgnr;
};

#endif // TASKRMREADCHANNELALIAS_H

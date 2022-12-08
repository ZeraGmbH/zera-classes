#ifndef TASKRMCHECKCHANNELSAVAIL_H
#define TASKRMCHECKCHANNELSAVAIL_H

#include "taskcomposit.h"
#include "rminterface.h"
#include <QStringList>

class TaskRmCheckChannelsAvail : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(Zera::Server::RMInterfacePtr rmInterface, QStringList expectedChannels);
    TaskRmCheckChannelsAvail(Zera::Server::RMInterfacePtr rmInterface, QStringList expectedChannels);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    QStringList m_expectedChannels;
    quint32 m_msgnr;
};

#endif // TASKRMCHECKCHANNELSAVAIL_H

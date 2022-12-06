#ifndef TASKRMREADCHANNELS_H
#define TASKRMREADCHANNELS_H

#include "taskcomposit.h"
#include "rminterface.h"
#include <QStringList>

class TaskRmReadChannels : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskRmReadChannels> create(Zera::Server::RMInterfacePtr rmInterface, QStringList expectedChannels);
    TaskRmReadChannels(Zera::Server::RMInterfacePtr rmInterface, QStringList expectedChannels);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    QStringList m_expectedChannels;
    quint32 m_msgnr;
};

#endif // TASKRMREADCHANNELS_H

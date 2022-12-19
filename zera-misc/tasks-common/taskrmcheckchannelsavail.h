#ifndef TASKRMCHECKCHANNELSAVAIL_H
#define TASKRMCHECKCHANNELSAVAIL_H

#include "taskcomposit.h"
#include "rmabstractinterface.h"
#include <QStringList>

class TaskRmCheckChannelsAvail : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(RmAbstractInterfacePtr rmInterface,
                                   QStringList expectedChannels,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRmCheckChannelsAvail(RmAbstractInterfacePtr rmInterface,
                             QStringList expectedChannels);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    RmAbstractInterfacePtr m_rmInterface;
    QStringList m_expectedChannels;
    quint32 m_msgnr;
};

#endif // TASKRMCHECKCHANNELSAVAIL_H

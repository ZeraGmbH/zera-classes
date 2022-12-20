#ifndef TASKRMCHECKCHANNELSAVAIL_H
#define TASKRMCHECKCHANNELSAVAIL_H

#include "taskcomposit.h"
#include "abstractrminterface.h"
#include <QStringList>

class TaskRmCheckChannelsAvail : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(AbstractRmInterfacePtr rmInterface,
                                   QStringList expectedChannels,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskRmCheckChannelsAvail(AbstractRmInterfacePtr rmInterface,
                             QStringList expectedChannels);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    AbstractRmInterfacePtr m_rmInterface;
    QStringList m_expectedChannels;
    quint32 m_msgnr;
};

#endif // TASKRMCHECKCHANNELSAVAIL_H

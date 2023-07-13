#ifndef TASKREGISTERREFCONSTCHANGENOTIFICATIONS_H
#define TASKREGISTERREFCONSTCHANGENOTIFICATIONS_H

#include <taskcontainersequence.h>
#include <pcbinterface.h>

class TaskRegisterRefConstChangeNotifications : public QObject
{
    Q_OBJECT
public:
    struct PowerRefInput
    {
        QString refInputName;
        int refInputInterruptNotifyId;
    };
    void startRegistrationTasks(Zera::PcbInterfacePtr pcbInterface, QList<PowerRefInput> refInputs);
signals:
    void sigRegistrationOk();
    void sigRegistrationFail();
private slots:
    void onRegistrationFinish(bool ok);
private:
    TaskContainerSequence m_registerTasks;
};

#endif // TASKREGISTERREFCONSTCHANGENOTIFICATIONS_H

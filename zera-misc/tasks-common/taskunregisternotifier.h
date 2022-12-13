#ifndef TASKUNREGISTERNOTIFIER_H
#define TASKUNREGISTERNOTIFIER_H

#include "taskcomposit.h"
#include "pcbinterface.h"

class TaskUnregisterNotifier : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface);
    static std::unique_ptr<TaskComposite> create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                 int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskUnregisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    quint32 m_msgnr;
};

#endif // TASKUNREGISTERNOTIFIER_H

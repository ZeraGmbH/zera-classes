#ifndef TASKUNREGISTERNOTIFIER_H
#define TASKUNREGISTERNOTIFIER_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskUnregisterNotifier : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::Server::PcbInterfacePtr pcbInterface,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskUnregisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(QVariant answer) override;
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    quint32 m_msgnr;
};

#endif // TASKUNREGISTERNOTIFIER_H

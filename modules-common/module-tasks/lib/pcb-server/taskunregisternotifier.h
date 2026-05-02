#ifndef TASKUNREGISTERNOTIFIER_H
#define TASKUNREGISTERNOTIFIER_H

#include "taskservertransactiontemplate.h"
#include <pcbinterface.h>

class TaskUnregisterNotifier : public TaskServerTransactionTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(const Zera::PcbInterfacePtr &pcbInterface,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskUnregisterNotifier(const Zera::PcbInterfacePtr &pcbInterface);
private:
    quint32 sendToServer() override;
    bool handleCheckedServerAnswer(const QVariant &answer) override;
    Zera::PcbInterfacePtr m_pcbInterface;
};

#endif // TASKUNREGISTERNOTIFIER_H

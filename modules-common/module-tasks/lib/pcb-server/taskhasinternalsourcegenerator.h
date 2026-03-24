#ifndef TASKHASINTERNALSOURCEGENERATOR_H
#define TASKHASINTERNALSOURCEGENERATOR_H

#include <taskservertransactiontemplate.h>

class TaskHasInternalSourceGenerator : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(AbstractServerInterfacePtr interface,
                                  std::shared_ptr<bool> hasInternalSourceGenerator,
                                  int timeout = TRANSACTION_TIMEOUT, std::function<void()> additionalErrorHandler = []{});
    TaskHasInternalSourceGenerator(AbstractServerInterfacePtr interface, std::shared_ptr<bool> hasInternalSourceGenerator);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, const QVariant &answer);
private:
    AbstractServerInterfacePtr m_interface;
    std::shared_ptr<bool> m_hasInternalSourceGenerator;
    quint32 m_msgnr = 0;
};

#endif // TASKHASINTERNALSOURCEGENERATOR_H

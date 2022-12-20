#ifndef TASKSERVERTRANSACTIONTEMPLATE_H
#define TASKSERVERTRANSACTIONTEMPLATE_H

#include "taskcomposit.h"
#include "abstractserverInterface.h"
#include <QAbstractSocket>
#include <memory>

class TaskServerTransactionTemplate : public TaskComposite
{
    Q_OBJECT
public:
    TaskServerTransactionTemplate(AbstractServerInterfacePtr server);
    void start() override;
    virtual quint32 sendToServer() = 0;
protected:
    virtual bool handleServerAnswer(QVariant answer) = 0;

private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void onServerError(QAbstractSocket::SocketError);
private:
    quint32 m_msgnr = 0;
    AbstractServerInterfacePtr m_server;
};

#endif // TASKSERVERTRANSACTIONTEMPLATE_H

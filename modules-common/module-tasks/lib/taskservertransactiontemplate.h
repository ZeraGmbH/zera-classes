#ifndef TASKSERVERTRANSACTIONTEMPLATE_H
#define TASKSERVERTRANSACTIONTEMPLATE_H

#include "tasktemplate.h"
#include "abstractserverInterface.h"
#include <QAbstractSocket>

class TaskServerTransactionTemplate : public TaskTemplate
{
    Q_OBJECT
public:
    TaskServerTransactionTemplate(AbstractServerInterfacePtr server);
    void start() override;
protected:
    AbstractServerInterfacePtr m_interface;

private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void onServerError(QAbstractSocket::SocketError);
private:
    virtual quint32 sendToServer() = 0;
    virtual bool handleCheckedServerAnswer(QVariant answer) = 0;
    quint32 m_msgnr = 0;
};

#endif // TASKSERVERTRANSACTIONTEMPLATE_H

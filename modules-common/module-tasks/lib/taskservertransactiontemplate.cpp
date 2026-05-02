#include "taskservertransactiontemplate.h"
#include <reply.h>

TaskServerTransactionTemplate::TaskServerTransactionTemplate(const AbstractServerInterfacePtr &serverInterface) :
    m_interface(serverInterface)
{
}

void TaskServerTransactionTemplate::start()
{
    connect(m_interface.get(), &AbstractServerInterface::serverAnswer,
            this, &TaskServerTransactionTemplate::onServerAnswer);
    connect(m_interface.get(), &AbstractServerInterface::tcpError,
            this, &TaskServerTransactionTemplate::onServerError);
    m_msgnr = sendToServer();
}

void TaskServerTransactionTemplate::onServerAnswer(quint32 msgnr, quint8 reply, const QVariant &answer)
{
    if(m_msgnr == msgnr) {
        bool ok = (reply == ack);
        if (ok)
            ok = handleCheckedServerAnswer(answer);
        finishTask(ok);
    }
}

void TaskServerTransactionTemplate::onServerError(QAbstractSocket::SocketError)
{
    finishTask(false);
}

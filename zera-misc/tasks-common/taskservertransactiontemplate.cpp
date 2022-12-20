#include "taskservertransactiontemplate.h"
#include "reply.h"

TaskServerTransactionTemplate::TaskServerTransactionTemplate(AbstractServerInterfacePtr server) :
    m_server(server)
{
}

void TaskServerTransactionTemplate::start()
{
    connect(m_server.get(), &AbstractServerInterface::serverAnswer,
            this, &TaskServerTransactionTemplate::onServerAnswer,
            Qt::QueuedConnection);
    connect(m_server.get(), &AbstractServerInterface::tcpError,
            this, &TaskServerTransactionTemplate::onServerError,
            Qt::QueuedConnection);
    m_msgnr = sendToServer();
}

void TaskServerTransactionTemplate::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        bool ok = (reply == ack);
        if (ok)
            ok = handleServerAnswer(answer);
        finishTask(ok);
    }
}

void TaskServerTransactionTemplate::onServerError(QAbstractSocket::SocketError)
{
    finishTask(false);
}

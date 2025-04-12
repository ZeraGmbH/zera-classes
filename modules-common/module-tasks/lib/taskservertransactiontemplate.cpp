#include "taskservertransactiontemplate.h"
#include <reply.h>

TaskServerTransactionTemplate::TaskServerTransactionTemplate(AbstractServerInterfacePtr server) :
    m_interface(server)
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

void TaskServerTransactionTemplate::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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

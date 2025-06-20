#include "taskgetinternalsourcecapabilities.h"
#include "taskdecoratortimeout.h"
#include <reply.h>
#include <QJsonDocument>

TaskTemplatePtr TaskGetInternalSourceCapabilities::create(Zera::PcbInterfacePtr pcbInterface,
                                                          std::shared_ptr<QJsonObject> jsonCapabilities,
                                                          int timeout,
                                                          std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskGetInternalSourceCapabilities>(pcbInterface, jsonCapabilities),
                                             additionalErrorHandler);
}

TaskGetInternalSourceCapabilities::TaskGetInternalSourceCapabilities(Zera::PcbInterfacePtr pcbInterface,
                                                                     std::shared_ptr<QJsonObject> jsonCapabilities) :
    m_pcbInterface(pcbInterface),
    m_jsonCapabilities(jsonCapabilities)
{
}

void TaskGetInternalSourceCapabilities::start()
{
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
            this, &TaskGetInternalSourceCapabilities::onServerAnswer);
    connect(m_pcbInterface.get(), &AbstractServerInterface::tcpError,
            this, &TaskGetInternalSourceCapabilities::onServerError);
    m_msgnr = sendToServer();
}

quint32 TaskGetInternalSourceCapabilities::sendToServer()
{
    return m_pcbInterface->scpiCommand("UISRC:CAPABILITIES?");
}

void TaskGetInternalSourceCapabilities::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack)
            *m_jsonCapabilities = QJsonDocument::fromJson(answer.toString().toUtf8()).object();
        finishTask(true);
    }
}

void TaskGetInternalSourceCapabilities::onServerError(QAbstractSocket::SocketError)
{
    finishTask(false);
}

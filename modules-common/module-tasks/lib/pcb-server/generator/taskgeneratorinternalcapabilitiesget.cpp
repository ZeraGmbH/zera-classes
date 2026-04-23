#include "taskgeneratorinternalcapabilitiesget.h"
#include "taskdecoratortimeout.h"
#include <reply.h>
#include <QJsonDocument>

TaskTemplatePtr TaskGeneratorInternalCapabilitiesGet::create(Zera::PcbInterfacePtr pcbInterface,
                                                             std::shared_ptr<QJsonObject> jsonCapabilities,
                                                             std::function<void ()> additionalErrorHandler, int timeout)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskGeneratorInternalCapabilitiesGet>(pcbInterface, jsonCapabilities),
                                             additionalErrorHandler);
}

TaskGeneratorInternalCapabilitiesGet::TaskGeneratorInternalCapabilitiesGet(Zera::PcbInterfacePtr pcbInterface,
                                                                           std::shared_ptr<QJsonObject> jsonCapabilities) :
    m_pcbInterface(pcbInterface),
    m_jsonCapabilities(jsonCapabilities)
{
}

void TaskGeneratorInternalCapabilitiesGet::start()
{
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
            this, &TaskGeneratorInternalCapabilitiesGet::onServerAnswer);
    connect(m_pcbInterface.get(), &AbstractServerInterface::tcpError,
            this, &TaskGeneratorInternalCapabilitiesGet::onServerError);
    m_msgnr = sendToServer();
}

quint32 TaskGeneratorInternalCapabilitiesGet::sendToServer()
{
    return m_pcbInterface->scpiCommand("GENERATOR:CAPABILITIES?");
}

void TaskGeneratorInternalCapabilitiesGet::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack)
            *m_jsonCapabilities = QJsonDocument::fromJson(answer.toString().toUtf8()).object();
        finishTask(true);
    }
}

void TaskGeneratorInternalCapabilitiesGet::onServerError(QAbstractSocket::SocketError)
{
    finishTask(false);
}

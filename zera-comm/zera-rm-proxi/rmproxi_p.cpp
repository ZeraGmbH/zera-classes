#include <QString>
#include <QUuid>
#include <QStateMachine>
#include <QState>

#include "rmproxi_p.h"
#include "interface_p.h"


namespace Zera
{
namespace RMProxi
{


cRMProxi* cRMProxiPrivate::singletonInstance=0;

cRMProxiPrivate::cRMProxiPrivate(cRMProxi *parent):
    q_ptr(parent)
{
    m_pStateMachine = new QStateMachine(this);

    m_pStateConnect = new QState();
    m_pStateIdent = new QState();

    m_pStateConnect->addTransition(this, SIGNAL(connected()), m_pStateIdent);
    m_pStateMachine->setInitialState(m_pStateConnect);

    connect(m_pStateIdent, SIGNAL(entered()), this, SLOT(entryIdent()));
    connect(this, SIGNAL(messageAvailable(QByteArray)), this, SLOT(receiveMessage(QByteArray)));

    m_pStateMachine->start();
}


cInterface *cRMProxiPrivate::getInterface()
{
    cInterfacePrivate* piface = new cInterfacePrivate(this);
    connect(piface, SIGNAL(rmCommand(ProtobufMessage::NetMessage*)), this, SLOT(transferCommand(ProtobufMessage::NetMessage*)));
    return piface;
}


void cRMProxiPrivate::transferCommand(ProtobufMessage::NetMessage *message)
{
    QUuid uuid;
    QByteArray key;

    uuid.createUuid();
    key = uuid.toRfc4122();
    message->set_uuid(key);
    m_UUIDHash[key] = qobject_cast<cInterfacePrivate*>(sender());
    sendMessage(message);
}


void cRMProxiPrivate::receiveMessage(QByteArray message)
{
    ProtobufMessage::NetMessage *netMessage = new ProtobufMessage::NetMessage();
    readMessage(netMessage, message);

    QByteArray key(netMessage->uuid().c_str(), netMessage->uuid().size());
    if (m_UUIDHash.contains(key))
    {
        Zera::RMProxi::cInterfacePrivate* iface = m_UUIDHash.take(key);
        iface->transferAnswer(netMessage);
    }
}


void cRMProxiPrivate::entryIdent()
{
    QString ident = proxiName;

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::NetReply* message = envelope.mutable_reply();
    message->set_rtype(ProtobufMessage::NetMessage::NetReply::IDENT);
    message->set_body(ident.toStdString());

    Zera::NetClient::cClientNetBase::sendMessage(&envelope);
}


}
}


#include <netmessages.pb.h>

#include "rminterface_p.h"
#include "rminterface.h"

namespace Zera
{
namespace Server
{


cRMInterfacePrivate::cRMInterfacePrivate(cRMInterface *iface)
    :q_ptr(iface)
{
    m_pClient = 0;
}


void cRMInterfacePrivate::setClient(Proxy::cProxyClient *client)
{
    if (m_pClient) // we avoid multiple connections
        disconnect(m_pClient, 0, this, 0);
    m_pClient = client;
    connect(m_pClient, SIGNAL(answerAvailable(ProtobufMessage::NetMessage*)), this, SLOT(receiveAnswer(ProtobufMessage::NetMessage*)));
    connect(m_pClient, SIGNAL(tcpError(QAbstractSocket::SocketError)), this, SLOT(receiveError(QAbstractSocket::SocketError)));
}


quint32 cRMInterfacePrivate::rmIdent(QString name)
{
    QString cmd;
    quint32 msgnr;

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::NetReply* message = envelope.mutable_reply();
    message->set_rtype(ProtobufMessage::NetMessage::NetReply::IDENT);
    message->set_body(name.toStdString());

    msgnr = m_pClient->transmitCommand(&envelope);
    m_MsgNrCmdList[msgnr] = rmident;

    return msgnr;
}


quint32 cRMInterfacePrivate::addResource(QString type, QString name, int n, QString description, quint16 port)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:ADD"), par = QString("%1;%2;%3;%4;%5;")
            .arg(type)
            .arg(name)
            .arg(n)
            .arg(description)
            .arg(port));

    m_MsgNrCmdList[msgnr] = addresource;
    return msgnr;
}


quint32 cRMInterfacePrivate::removeResource(QString type, QString name)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:REM"), par = QString("%1;%2;")
            .arg(type)
            .arg(name));

    m_MsgNrCmdList[msgnr] = removeresource;
    return msgnr;
}


quint32 cRMInterfacePrivate::getResourceTypes()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:TYPE:CAT?"));

    m_MsgNrCmdList[msgnr] = getresourcetypes;
    return msgnr;
}


quint32 cRMInterfacePrivate::getResources(QString type)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:%1:CAT?").arg(type));

    m_MsgNrCmdList[msgnr] = getresources;
    return msgnr;
}


quint32 cRMInterfacePrivate::getResourceInfo(QString type, QString name)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:%1:%2?").arg(type).arg(name));

    m_MsgNrCmdList[msgnr] = getresources;
    return msgnr;
}


quint32 cRMInterfacePrivate::setResource(QString type, QString name, int n)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:%1:%2").arg(type).arg(name), par = QString("SET;%1;").arg(n));

    m_MsgNrCmdList[msgnr] = setresource;
    return msgnr;
}


quint32 cRMInterfacePrivate::freeResource(QString type, QString name)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:%1:%2").arg(type).arg(name) , par = QString("FREE"));

    m_MsgNrCmdList[msgnr] = freeresource;
    return msgnr;
}


void cRMInterfacePrivate::receiveAnswer(ProtobufMessage::NetMessage *message)
{
    if (message->has_reply())
    {
        quint32 lmsgnr;
        QString lmsg = "";
        int lreply;

        lmsgnr = message->messagenr();

        if (message->reply().has_body())
        {
            lmsg = QString::fromStdString(message->reply().body());
        }

        lreply = message->reply().rtype();

        int lastCmd = m_MsgNrCmdList.take(lmsgnr);

        Q_Q(cRMInterface);

        switch (lastCmd)
        {
        case rmident:
        case addresource:
        case removeresource:
        case setresource:
        case freeresource:
            emit q->serverAnswer(lmsgnr, lreply, returnString(lmsg));
            break;
        case getresourcetypes:
        case getresources:
        case getresourceinfo:
            emit q->serverAnswer(lmsgnr, lreply, returnString(lmsg));
            break;
        }
    }
}


void cRMInterfacePrivate::receiveError(QAbstractSocket::SocketError errorCode)
{
    Q_Q(cRMInterface);
    emit q->tcpError(errorCode);
}

}
}


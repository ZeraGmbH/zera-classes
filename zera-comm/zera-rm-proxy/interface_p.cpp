#include "interface_p.h"


namespace Zera
{
namespace RMProxy
{


cInterfacePrivate::cInterfacePrivate(QObject *parent)
{
    setParent(parent);
}


void cInterfacePrivate::addResource(QString type, QString name, int n, QString description, quint16 port)
{
    QString cmd, par;

    cmd = "RES:ADD";
    par = QString("%1;%2;%3;%4;%5;")
            .arg(type)
            .arg(name)
            .arg(n)
            .arg(description)
            .arg(port);

    sendCommand(cmd, par);
}


void cInterfacePrivate::removeResource(QString type, QString name)
{
    QString cmd, par;

    cmd = "RES:REM";
    par = QString("%1;%2;")
            .arg(type)
            .arg(name);

    sendCommand(cmd, par);
}


void cInterfacePrivate::getResourceTypes()
{
    QString cmd;

    cmd = "RES:TYPE:CAT?";
    sendCommand(cmd);
}


void cInterfacePrivate::getResources(QString type)
{
    QString cmd;

    cmd = QString("RES:%1:CAT?").arg(type);
    sendCommand(cmd);
}


void cInterfacePrivate::getResourceInfo(QString type, QString name)
{
    QString cmd;

    cmd = QString("RES:%1:%2?").arg(type).arg(name);
    sendCommand(cmd);
}


void cInterfacePrivate::setResource(QString type, QString name, int n)
{
    QString cmd, par;

    cmd = QString("RES:%1:%2?").arg(type).arg(name);
    par = QString("SET;%1;").arg(n);
    sendCommand(cmd, par);
}


void cInterfacePrivate::freeResource(QString type, QString name)
{
    QString cmd, par;

    cmd = QString("RES:%1:%2?").arg(type).arg(name);
    par = "FREE";
    sendCommand(cmd, par);
}


void cInterfacePrivate::transferAnswer(ProtobufMessage::NetMessage *message)
{
    if (message->has_reply())
    {
        if (message->reply().has_body())
            emit rmAnswer(QString::fromStdString(message->reply().body()));
        else
        {
            int n = message->reply().rtype();
            emit rmReply(replies(n));
        }
    }
}


void cInterfacePrivate::sendCommand(QString cmd)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    emit rmCommand(&envelope);
}


void cInterfacePrivate::sendCommand(QString cmd, QString par)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());
    emit rmCommand(&envelope);
}



}
}


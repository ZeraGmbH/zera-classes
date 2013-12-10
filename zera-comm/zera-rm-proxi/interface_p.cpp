#include "interface_p.h"


namespace Zera
{
namespace RMProxi
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

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());

    emit rmCommand(&envelope);
}


void cInterfacePrivate::removeResource(QString type, QString name)
{
    QString cmd, par;

    cmd = "RES:REM";
    par = QString("%1;%2;")
            .arg(type)
            .arg(name);

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());

    emit rmCommand(&envelope);
}


void cInterfacePrivate::getResourceTypes()
{
    QString cmd;

    cmd = "RES:TYPE:CAT?";

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());

    emit rmCommand(&envelope);
}


void cInterfacePrivate::getResources(QString type)
{
    QString cmd;

    cmd = QString("RES:%1:CAT?").arg(type);

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());

    emit rmCommand(&envelope);
}


void cInterfacePrivate::getResourceInfo(QString type, QString name)
{
    QString cmd;

    cmd = QString("RES:%1:%2?").arg(type).arg(name);

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());

    emit rmCommand(&envelope);
}


void cInterfacePrivate::setResource(QString type, QString name, int n)
{
    QString cmd, par;

    cmd = QString("RES:%1:%2?").arg(type).arg(name);
    par = QString("SET;%1;").arg(n);

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());

    emit rmCommand(&envelope);
}


void cInterfacePrivate::freeResource(QString type, QString name)
{
    QString cmd, par;

    cmd = QString("RES:%1:%2?").arg(type).arg(name);
    par = "FREE";

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());

    emit rmCommand(&envelope);
}


void cInterfacePrivate::transferAnswer(ProtobufMessage::NetMessage *message)
{
    if (message->has_reply())
    {
        int n = message->reply().rtype();
        emit rmReply(replies(n));
        if (message->reply().has_body())
            emit rmAnswer(QString::fromStdString(message->reply().body()));
    }
}

}
}


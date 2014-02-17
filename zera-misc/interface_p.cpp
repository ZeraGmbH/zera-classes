#include <QStringList>

#include "interface_p.h"

namespace Zera
{
namespace Server
{

quint32 cInterfacePrivate::sendCommand(QString cmd)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    return m_pClient->transmitCommand(&envelope);
}


quint32 cInterfacePrivate::sendCommand(QString cmd, QString par)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());
    return m_pClient->transmitCommand(&envelope);
}


QVariant cInterfacePrivate::returnInt(QString message)
{
    bool ok;
    qint32 n;

    n = message.toInt(&ok);
    return QVariant(n);
}


QVariant cInterfacePrivate::returnString(QString message)
{
    return QVariant(message);
}


QVariant cInterfacePrivate::returnStringList(QString message)
{
    QString sep = ";";
    return QVariant(message.split(sep));
}


QVariant cInterfacePrivate::returnDouble(QString message)
{
    bool ok;
    double d;

    d = message.toDouble(&ok);
    return QVariant(d);
}


QVariant cInterfacePrivate::returnBool(QString message)
{
    bool ok, avail;

    avail = (message.toInt(&ok) == 1);
    return QVariant(avail);
}


QVariant cInterfacePrivate::returnReply( int reply)
{
    return replies(reply);
}

}
}




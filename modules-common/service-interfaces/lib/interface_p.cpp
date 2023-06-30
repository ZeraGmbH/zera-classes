#include <QStringList>

#include "interface_p.h"

namespace Zera {

bool cInterfacePrivate::decodeProtobuffAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message,
                                              TAnswerDecoded &decodedAnswer,
                                              int interruptEnumVal)
{
    bool withReply = message->has_reply();
    if(withReply) {
        decodedAnswer.msgNr = message->messagenr();
        if (message->reply().has_body())
            decodedAnswer.msgBody = QString::fromStdString(message->reply().body());
        decodedAnswer.reply = message->reply().rtype();
        if(interruptEnumVal != -1 && decodedAnswer.msgNr == 0)
            decodedAnswer.cmdSendEnumVal = interruptEnumVal;
        else
            decodedAnswer.cmdSendEnumVal = m_MsgNrCmdList.take(decodedAnswer.msgNr);
    }
    return withReply;
}

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



}


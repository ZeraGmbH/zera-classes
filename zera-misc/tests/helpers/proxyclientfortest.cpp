#include "proxyclientfortest.h"
#include <reply.h>

std::shared_ptr<ProxyClientForTest> ProxyClientForTest::create()
{
    return std::make_shared<ProxyClientForTest>();
}

void ProxyClientForTest::setAnswers(RmTestAnswers answers)
{
    m_answers = answers;
}

quint32 ProxyClientForTest::transmitCommand(ProtobufMessage::NetMessage *message)
{
    storeMessage(message);
    if(!m_answers.isEmpty()) {
        RmTestAnswer answer = m_answers.take();
        if(answer.answerType !=RmTestAnswer::TCP_ERROR)
            return sendAnswer(message, answer);
        else {
            emit tcpError(QAbstractSocket::RemoteHostClosedError);
            return 0;
        }
    }
    return 0;
}

QStringList ProxyClientForTest::getReceivedCommands() const
{
    return m_receivedCommands;
}

quint32 ProxyClientForTest::sendAnswer(ProtobufMessage::NetMessage *message, RmTestAnswer answer)
{
    std::shared_ptr<ProtobufMessage::NetMessage> answerMessage = std::make_shared<ProtobufMessage::NetMessage>(*message);
    quint32 messageNr = calcMessageNr(answer, answerMessage.get());

    ProtobufMessage::NetMessage::NetReply *answerReply = answerMessage->mutable_reply();
    answerReply->set_body(answer.answer.toString().toStdString());
    setReply(answerReply, answer);

    emit answerAvailable(answerMessage);
    return messageNr;
}

void ProxyClientForTest::storeMessage(ProtobufMessage::NetMessage *message)
{
    ProtobufMessage::NetMessage::ScpiCommand scpiCmd = message->scpi();
    QString strCmd = QString::fromStdString(scpiCmd.command());
    QString param = QString::fromStdString(scpiCmd.parameter());
    if(!param.isEmpty())
        strCmd += " " + param;
    m_receivedCommands.append(strCmd);
}

quint32 ProxyClientForTest::calcMessageNr(RmTestAnswer answer, ProtobufMessage::NetMessage* answerMessage)
{
    quint32 messageNr = 0;
    switch(answer.answerType)
    {
    default:
        messageNr = m_msgIds.nextId();
        answerMessage->set_messagenr(messageNr);
        break;
    case RmTestAnswer::MSG_ID_OTHER:
        messageNr = m_msgIds.nextId();
        answerMessage->set_messagenr(-messageNr);
        break;
    case RmTestAnswer::MSG_ID_INTERRUPT:
        messageNr = 0;
        answerMessage->set_messagenr(messageNr);
        break;
    }
    return messageNr;
}

void ProxyClientForTest::setReply(ProtobufMessage::NetMessage::NetReply *answerReply, RmTestAnswer answer)
{
    switch(answer.reply) {
    case ack:
        answerReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
        break;
    case nack:
        answerReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
        break;
    default:
        answerReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
        break;
    }
}


#ifndef PROXYCLIENTFORTEST_H
#define PROXYCLIENTFORTEST_H

#include "rmtestanswers.h"
#include "msgidgenerator.h"
#include <proxy.h>
#include <netmessages.pb.h>

class ProxyClientForTest : public Zera::Proxy::cProxyClient
{
    Q_OBJECT
public:
    static std::shared_ptr<ProxyClientForTest> create();
    void setAnswers(RmTestAnswers answers);
    quint32 transmitCommand(ProtobufMessage::NetMessage *message) override;

private:
    quint32 sendAnswer(ProtobufMessage::NetMessage *message, RmTestAnswer answer);
    void storeMessage(ProtobufMessage::NetMessage *message);
    quint32 calcMessageNr(RmTestAnswer answer, ProtobufMessage::NetMessage *answerMessage);
    void setReply(ProtobufMessage::NetMessage::NetReply *answerReply, RmTestAnswer answer);
    MsgIdGenerator m_msgIds;
    RmTestAnswers m_answers;
    QStringList m_receivedCommands;
};

typedef std::shared_ptr<ProxyClientForTest> ProxyClientForTestPtr;

#endif // PROXYCLIENTFORTEST_H

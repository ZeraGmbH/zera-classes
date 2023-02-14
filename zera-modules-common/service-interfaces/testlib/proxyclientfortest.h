#ifndef PROXYCLIENTFORTEST_H
#define PROXYCLIENTFORTEST_H

#include "servertestanswers.h"
#include "msgidgenerator.h"
#include <proxy.h>
#include <netmessages.pb.h>

namespace Zera {

class ProxyClientForTest : public ProxyClient
{
    Q_OBJECT
public:
    static std::shared_ptr<ProxyClientForTest> create();
    ProxyClientForTest();
    void setAnswers(ServerTestAnswers answers);
    quint32 transmitCommand(ProtobufMessage::NetMessage *message) override;
    QStringList getReceivedIdents() const;
    QStringList getReceivedCommands() const;
signals:
    void sigQueueAnswer();
private slots:
    void onQueueAnswer();
private:
    quint32 pushAnswer(ProtobufMessage::NetMessage *message, ServerTestAnswer answer);
    void storeMessage(ProtobufMessage::NetMessage *message);
    void storeScpi(ProtobufMessage::NetMessage *message);
    quint32 calcMessageNr(ServerTestAnswer answer, ProtobufMessage::NetMessage *answerMessage);
    void setReply(ProtobufMessage::NetMessage::NetReply *answerReply, ServerTestAnswer answer);
    MsgIdGenerator m_msgIds;
    ServerTestAnswers m_answers;
    QList<std::shared_ptr<ProtobufMessage::NetMessage>> m_pendingNetAnswers;
    QStringList m_receivedIdents;
    QStringList m_receivedCommands;
};

typedef std::shared_ptr<ProxyClientForTest> ProxyClientForTestPtr;

}

#endif // PROXYCLIENTFORTEST_H

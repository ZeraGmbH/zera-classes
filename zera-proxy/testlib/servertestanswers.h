#ifndef SERVERTESTANSWERS_H
#define SERVERTESTANSWERS_H

#include <reply.h>
#include <QVariant>
#include <QList>

struct ServerTestAnswer
{
    enum AnswerType
    {
        MSG_ID_MATCH,
        MSG_ID_INTERRUPT,
        MSG_ID_OTHER,
        TCP_ERROR
    };
    ServerTestAnswer(quint8 reply, QVariant answer, AnswerType answerType = MSG_ID_MATCH);
    quint8 reply;
    QVariant answer;
    AnswerType answerType;
};

typedef QList<ServerTestAnswer> ServerTestAnswerList;

class ServerTestAnswers
{
public:
    ServerTestAnswers();
    ServerTestAnswers(ServerTestAnswerList answers);
    ServerTestAnswer take();
    bool isEmpty();
private:
    ServerTestAnswerList m_answers;
};

#endif // SERVERTESTANSWERS_H

#ifndef RMTESTANSWERS_H
#define RMTESTANSWERS_H

#include "reply.h"
#include <QVariant>
#include <QList>

struct RmTestAnswer
{
    enum AnswerType
    {
        MSG_ID_MATCH,
        MSG_ID_INTERRUPT,
        MSG_ID_OTHER,
        TCP_ERROR
    };
    RmTestAnswer(quint8 reply, QVariant answer, AnswerType answerType = MSG_ID_MATCH);
    quint8 reply;
    QVariant answer;
    AnswerType answerType;
};

typedef QList<RmTestAnswer> RmTestAnswerList;

class RmTestAnswers
{
public:
    RmTestAnswers();
    RmTestAnswers(RmTestAnswerList answers);
    RmTestAnswer take();
    bool isEmpty();
private:
    RmTestAnswerList m_answers;
};

#endif // RMTESTANSWERS_H

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

class RmTestAnswers
{
public:
    RmTestAnswers();
    RmTestAnswers(QList<RmTestAnswer> answers);
    RmTestAnswer take();
private:
    QList<RmTestAnswer> m_answers;
};

#endif // RMTESTANSWERS_H

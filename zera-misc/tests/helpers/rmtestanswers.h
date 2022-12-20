#ifndef RMTESTANSWERS_H
#define RMTESTANSWERS_H

#include "reply.h"
#include <QVariant>
#include <QList>

struct RmTestAnswer
{
    enum MsgIdType
    {
        DEFAULT,
        INTERRUPT,
        OTHER
    };
    RmTestAnswer(quint8 reply, QVariant answer, MsgIdType msgIdType = DEFAULT);
    quint8 reply;
    QVariant answer;
    MsgIdType msgIdType;
};

class RmTestAnswers
{
public:
    RmTestAnswers(QList<RmTestAnswer> answers);
    RmTestAnswer take();
private:
    QList<RmTestAnswer> m_answers;
};

#endif // RMTESTANSWERS_H

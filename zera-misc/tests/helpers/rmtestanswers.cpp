#include "rmtestanswers.h"

RmTestAnswer::RmTestAnswer(quint8 reply, QVariant answer, MsgIdType msgIdType)
{
    this->reply = reply;
    this->answer = answer;
    this->msgIdType = msgIdType;
}

RmTestAnswers::RmTestAnswers(QList<RmTestAnswer> answers) :
    m_answers(answers)
{
}

RmTestAnswer RmTestAnswers::take()
{
    return m_answers.takeFirst();
}


#include "rmtestanswers.h"

RmTestAnswer::RmTestAnswer(quint8 reply, QVariant answer)
{
    this->reply = reply;
    this->answer = answer;
}

RmTestAnswers::RmTestAnswers(QList<RmTestAnswer> answers) :
    m_answers(answers)
{
}


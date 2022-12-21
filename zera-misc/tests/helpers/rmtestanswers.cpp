#include "rmtestanswers.h"

RmTestAnswer::RmTestAnswer(quint8 reply, QVariant answer, AnswerType answerType)
{
    this->reply = reply;
    this->answer = answer;
    this->answerType = answerType;
}

RmTestAnswers::RmTestAnswers()
{
}

RmTestAnswers::RmTestAnswers(RmTestAnswerList answers) :
    m_answers(answers)
{
}

RmTestAnswer RmTestAnswers::take()
{
    return m_answers.takeFirst();
}


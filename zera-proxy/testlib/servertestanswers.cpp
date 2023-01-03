#include "servertestanswers.h"

ServerTestAnswer::ServerTestAnswer(quint8 reply, QVariant answer, AnswerType answerType)
{
    this->reply = reply;
    this->answer = answer;
    this->answerType = answerType;
}

ServerTestAnswers::ServerTestAnswers()
{
}

ServerTestAnswers::ServerTestAnswers(ServerTestAnswerList answers) :
    m_answers(answers)
{
}

ServerTestAnswer ServerTestAnswers::take()
{
    return m_answers.takeFirst();
}

bool ServerTestAnswers::isEmpty()
{
    return m_answers.isEmpty();
}


#ifndef RMTESTANSWERS_H
#define RMTESTANSWERS_H

#include <QVariant>
#include <QList>

struct RmTestAnswer
{
    RmTestAnswer(quint8 reply, QVariant answer);
    quint8 reply;
    QVariant answer;
};

class RmTestAnswers
{
public:
    RmTestAnswers(QList<RmTestAnswer> answers);
private:
    QList<RmTestAnswer> m_answers;
};

#endif // RMTESTANSWERS_H

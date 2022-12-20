#include "rminterfacefortest.h"

AbstractRmInterfacePtr RmInterfaceForTest::create(QList<RmTestAnswer> answers)
{
    return std::make_shared<RmInterfaceForTest>(answers);
}

RmInterfaceForTest::RmInterfaceForTest(QList<RmTestAnswer> answers) :
    m_answers(answers)
{
}

quint32 RmInterfaceForTest::getResources(QString type)
{
    return sendAnswer();
}

quint32 RmInterfaceForTest::sendAnswer()
{
    RmTestAnswer answer = m_answers.take();
    quint32 msgIdSend, msgIdReturned;
    switch(answer.answerType) {
    case RmTestAnswer::MSG_ID_MATCH:
    case RmTestAnswer::TCP_ERROR:
        msgIdSend = msgIdReturned = m_msgIds.nextId();
        break;
    case RmTestAnswer::MSG_ID_INTERRUPT:
        msgIdSend = msgIdReturned = 0;
        break;
    case RmTestAnswer::MSG_ID_OTHER:
        msgIdSend = m_msgIds.nextId();
        msgIdReturned = m_msgIds.nextId();
        break;
    }
    if(answer.answerType != RmTestAnswer::TCP_ERROR)
        emit serverAnswer(msgIdSend, answer.reply, answer.answer);
    else
        emit tcpError(QAbstractSocket::RemoteHostClosedError);
    return msgIdReturned;
}

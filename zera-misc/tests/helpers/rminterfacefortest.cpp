#include "rminterfacefortest.h"

RmInterfaceForTest::RmInterfaceForTest(RmTestAnswers answers) :
    m_answers(answers)
{
    connect(this, &RmInterfaceForTest::serverAnswerQueued,
            this, &RmInterfaceForTest::serverAnswer,
            Qt::QueuedConnection);
}

quint32 RmInterfaceForTest::getResources(QString type)
{
    return sendAnswer();
}

quint32 RmInterfaceForTest::sendAnswer()
{
    RmTestAnswer answer = m_answers.take();
    quint32 msgIdSend, msgIdReturned;
    switch(answer.msgIdType) {
    case RmTestAnswer::DEFAULT:
        msgIdSend = msgIdReturned = m_msgIds.nextId();
        break;
    case RmTestAnswer::INTERRUPT:
        msgIdSend = msgIdReturned = 0;
        break;
    case RmTestAnswer::OTHER:
        msgIdSend = m_msgIds.nextId();
        msgIdReturned = m_msgIds.nextId();
        break;
    }
    emit serverAnswerQueued(msgIdSend, answer.reply, answer.answer);
    return msgIdReturned;
}

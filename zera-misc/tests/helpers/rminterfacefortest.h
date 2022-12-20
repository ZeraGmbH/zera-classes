#ifndef RMINTERFACEFORTEST_H
#define RMINTERFACEFORTEST_H

#include "rmtestanswers.h"
#include "rmabstractinterface.h"
#include "msgidgenerator.h"

class RmInterfaceForTest : public RmAbstractInterface
{
    Q_OBJECT
public:
    RmInterfaceForTest(RmTestAnswers answers);
    quint32 getResources(QString type) override;
private:
    quint32 sendAnswer();
    RmTestAnswers m_answers;
    MsgIdGenerator m_msgIds;
signals:
    void serverAnswerQueued(quint32 msgnr, quint8 reply, QVariant answer);
};

#endif // RMINTERFACEFORTEST_H

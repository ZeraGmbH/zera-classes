#ifndef RMINTERFACEFORTEST_H
#define RMINTERFACEFORTEST_H

#include "rmtestanswers.h"
#include "abstractrminterface.h"
#include "msgidgenerator.h"

class RmInterfaceForTest : public AbstractRmInterface
{
    Q_OBJECT
public:
    static AbstractRmInterfacePtr create(QList<RmTestAnswer> answers);
    RmInterfaceForTest(QList<RmTestAnswer> answers);
    quint32 getResources(QString type) override;
private:
    quint32 sendAnswer();
    RmTestAnswers m_answers;
    MsgIdGenerator m_msgIds;
};

#endif // RMINTERFACEFORTEST_H

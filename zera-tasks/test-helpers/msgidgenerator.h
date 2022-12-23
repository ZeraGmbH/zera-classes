#ifndef MSGIDGENERATOR_H
#define MSGIDGENERATOR_H

#include <QtGlobal>

class MsgIdGenerator
{
public:
    quint32 currId();
    quint32 nextId();
private:
    quint32 m_currMsgId = 0;
};

#endif // MSGIDGENERATOR_H

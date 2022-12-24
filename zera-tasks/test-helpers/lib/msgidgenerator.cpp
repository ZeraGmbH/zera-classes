#include "msgidgenerator.h"

quint32 MsgIdGenerator::currId()
{
    return m_currMsgId;
}

quint32 MsgIdGenerator::nextId()
{
    m_currMsgId++;
    if(m_currMsgId == 0)
        m_currMsgId++;
    return currId();
}

#include "scpitransactionid.h"

quint64 ScpiTransactionId::m_currentId = INVALID;

ScpiTransactionId::ScpiTransactionId()
{
}

ScpiTransactionId ScpiTransactionId::createUniqueId()
{
    m_currentId++;
    if (m_currentId == INVALID)
        m_currentId++;

    return ScpiTransactionId(m_currentId);
}

ScpiTransactionId::ScpiTransactionId(quint64 id) :
    m_id(id)
{
}

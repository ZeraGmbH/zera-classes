#include "scpitransactionid.h"

quint64 ScpiTransactionId::m_currentId = INVALID;

ScpiTransactionId::ScpiTransactionId()
{
}

ScpiTransactionId ScpiTransactionId::createUniqueId()
{
    m_currentId++;
    // If we assume a SCPI transaction per millisecond we can do similar assumptions as datetime
    // => No wraparound to expect in anyone's / anything's lifetime
    return ScpiTransactionId(m_currentId);
}

quint64 ScpiTransactionId::getChrono() const
{
    return m_id;
}

ScpiTransactionId::ScpiTransactionId(quint64 id) :
    m_id(id)
{
}

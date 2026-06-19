#include "scpitransactionid.h"

static constexpr quint64 INVALID = 0;

ScpiTransactionId::ScpiTransactionId() :
    m_id(INVALID)
{
}

ScpiTransactionId ScpiTransactionId::createUniqueId()
{
    static quint64 currentTransactionId = INVALID;
    currentTransactionId++;

    // If we assume a SCPI transaction per millisecond we can do similar assumptions as datetime
    // => No wraparound to expect in anyone's / anything's lifetime
    return ScpiTransactionId(currentTransactionId);
}

quint64 ScpiTransactionId::getChrono() const
{
    return m_id;
}

bool ScpiTransactionId::isValid() const
{
    return m_id != INVALID;
}

ScpiTransactionId::ScpiTransactionId(quint64 id) :
    m_id(id)
{
}

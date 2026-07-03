#include "scpitransactionid.h"

static constexpr quint64 INVALID = 0;

ScpiTransactionId::ScpiTransactionId() :
    m_id(INVALID)
{
}

ScpiTransactionId ScpiTransactionId::createUniqueId(const QString &scpi)
{
    static quint64 currentTransactionId = INVALID;
    currentTransactionId++;

    // If we assume a SCPI transaction per millisecond we can do similar assumptions as datetime
    // => No wraparound to expect in anyone's / anything's lifetime
    return ScpiTransactionId(currentTransactionId, scpi);
}

quint64 ScpiTransactionId::getChrono() const
{
    return m_id;
}

bool ScpiTransactionId::isValid() const
{
    return m_id != INVALID;
}

const QString &ScpiTransactionId::getScpi() const
{
    return m_scpi;
}

ScpiTransactionId::ScpiTransactionId(quint64 id, const QString &scpi) :
    m_id(id),
    m_scpi(scpi)
{
}

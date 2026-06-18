#ifndef SCPITRANSACTIONID_H
#define SCPITRANSACTIONID_H

#include <QtGlobal>

class ScpiTransactionId
{
public:
    ScpiTransactionId();
    static ScpiTransactionId createUniqueId();
    quint64 getChrono() const;
private:
    ScpiTransactionId(quint64 id);

    static constexpr quint64 INVALID = 0;
    quint64 m_id = INVALID;

    static quint64 m_currentId;
};

#endif // SCPITRANSACTIONID_H

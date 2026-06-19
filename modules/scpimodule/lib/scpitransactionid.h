#ifndef SCPITRANSACTIONID_H
#define SCPITRANSACTIONID_H

#include <QtGlobal>

class ScpiTransactionId
{
public:
    ScpiTransactionId();
    static ScpiTransactionId createUniqueId();
    quint64 getChrono() const;
    bool isValid() const;

private:
    ScpiTransactionId(quint64 id);
    quint64 m_id;
};

#endif // SCPITRANSACTIONID_H

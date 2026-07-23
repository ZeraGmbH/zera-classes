#ifndef SCPITRANSACTIONID_H
#define SCPITRANSACTIONID_H

#include <QString>

class ScpiTransactionId
{
public:
    ScpiTransactionId();
    static ScpiTransactionId createUniqueId(const QString &scpi);
    bool operator== (const ScpiTransactionId &other) const;

    quint64 getChrono() const;
    bool isValid() const;
    const QString &getScpi() const;

private:
    ScpiTransactionId(quint64 id, const QString &scpi);

    quint64 m_id;
    QString m_scpi;
};

#endif // SCPITRANSACTIONID_H

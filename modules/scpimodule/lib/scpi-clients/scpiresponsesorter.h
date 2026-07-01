#ifndef SCPIRESPONSESORTER_H
#define SCPIRESPONSESORTER_H

#include "scpitransactionid.h"
#include "nullablestring.h"
#include <QString>
#include <QMap>

class ScpiResponseSorter
{
public:
    ScpiTransactionId createTransaction(const QString &scpi);
    NullableStringList genOrDelaySortedOutput(const NullableString &scpiSingleResponse, const ScpiTransactionId &scpiTransactionId);

private:
    NullableStringList createAccumulatedResponse();

    QMap<quint64, ScpiTransactionId> m_transactionsPending;
    QMap<quint64, NullableString> m_transactionsFinished;
};

#endif // SCPIRESPONSESORTER_H

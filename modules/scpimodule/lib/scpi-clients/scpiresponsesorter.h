#ifndef SCPIRESPONSESORTER_H
#define SCPIRESPONSESORTER_H

#include "scpitransactionid.h"
#include <QString>
#include <QMap>

class ScpiResponseSorter
{
public:
    ScpiTransactionId createTransaction(const QString &scpi);
    QStringList genOrDelaySortedOutput(const QString &scpiSingleResponse, const ScpiTransactionId &scpiTransactionId);

private:
    QStringList createAccumulatedResponse();

    QMap<quint64, ScpiTransactionId> m_transactionsPending;
    QMap<quint64, QString> m_transactionsFinished;
};

#endif // SCPIRESPONSESORTER_H

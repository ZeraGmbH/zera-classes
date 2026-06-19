#ifndef SCPICMDRESPONSESORTER_H
#define SCPICMDRESPONSESORTER_H

#include "scpitransactionid.h"
#include <QByteArray>
#include <QMap>

class ScpiCmdResponseSorter
{
public:
    ScpiTransactionId createTransaction();
    QByteArray genOrDelaySortedOutput(const QByteArray& scpiSingleResponse, const ScpiTransactionId &scpiTransactionId);
private:
    QByteArray createAccumulatedResponse(const ScpiTransactionId &scpiCurrentTransactionId);

    QMap<quint64, ScpiTransactionId> m_transactionsPending;
    QMap<quint64, QByteArray> m_transactionsFinished;
};

#endif // SCPICMDRESPONSESORTER_H

#include "scpicmdresponsesorter.h"

ScpiTransactionId ScpiCmdResponseSorter::createTransaction()
{
    ScpiTransactionId id = ScpiTransactionId::createUniqueId();
    m_transactionsPending.insert(id.getChrono(), id);
    return id;
}

QByteArray ScpiCmdResponseSorter::genOrDelaySortedOutput(const QByteArray &scpiSingleResponse, const ScpiTransactionId &scpiTransactionId)
{
    const quint64 chrono = scpiTransactionId.getChrono();
    ScpiTransactionId pendingTransaction = m_transactionsPending.take(chrono);
    if (pendingTransaction.isValid()) {
        m_transactionsFinished.insert(chrono, scpiSingleResponse);
        return createAccumulatedResponse(scpiTransactionId);
    }
    return scpiSingleResponse;
}

QByteArray ScpiCmdResponseSorter::createAccumulatedResponse(const ScpiTransactionId &scpiCurrentTransactionId)
{
    QByteArray totalResponse;
    QList<quint64> toDeleteList;
    for (auto iter = m_transactionsFinished.cbegin(); iter != m_transactionsFinished.cend(); ++iter) {
        quint64 currentChrono = scpiCurrentTransactionId.getChrono();
        if (!m_transactionsPending.isEmpty()) {
            quint64 minimalPendingChrono = m_transactionsPending.firstKey();
            if (currentChrono > minimalPendingChrono)
                break;
        }
        totalResponse += iter.value();
        toDeleteList.append(iter.key());
    }
    for (const quint64 &toDelete : toDeleteList)
        m_transactionsFinished.remove(toDelete);
    return totalResponse;
}

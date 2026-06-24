#include "scpicmdresponsesorter.h"

ScpiTransactionId ScpiCmdResponseSorter::createTransaction()
{
    ScpiTransactionId id = ScpiTransactionId::createUniqueId();
    m_transactionsPending.insert(id.getChrono(), id);
    return id;
}

QStringList ScpiCmdResponseSorter::genOrDelaySortedOutput(const QString &scpiSingleResponse, const ScpiTransactionId &scpiTransactionId)
{
    const quint64 chrono = scpiTransactionId.getChrono();
    ScpiTransactionId pendingTransaction = m_transactionsPending.take(chrono);
    if (pendingTransaction.isValid()) {
        m_transactionsFinished.insert(chrono, scpiSingleResponse);
        return createAccumulatedResponse();
    }
    return QStringList() << scpiSingleResponse;
}

QStringList ScpiCmdResponseSorter::createAccumulatedResponse()
{
    QStringList responseList;
    QList<quint64> toDeleteList;
    for (auto iter = m_transactionsFinished.cbegin(); iter != m_transactionsFinished.cend(); ++iter) {
        quint64 currentChrono = iter.key();
        if (!m_transactionsPending.isEmpty()) {
            quint64 minimalPendingChrono = m_transactionsPending.firstKey();
            if (currentChrono > minimalPendingChrono)
                break;
        }
        responseList.append(iter.value());
        toDeleteList.append(iter.key());
    }
    for (const quint64 &toDelete : toDeleteList)
        m_transactionsFinished.remove(toDelete);
    return responseList;
}

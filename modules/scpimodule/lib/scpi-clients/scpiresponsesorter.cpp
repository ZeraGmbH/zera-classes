#include "scpiresponsesorter.h"
#include "scpimodulecommonstaticfunctions.h"

ScpiResponseSorter::~ScpiResponseSorter()
{
#ifdef QT_DEBUG // As long as production code does not sort this polutes logs
    if (m_transactionsPending.size()) {
        int unfinished = 0;
        for(auto iter=m_transactionsPending.cbegin(); iter!=m_transactionsPending.cend(); ++iter) {
            const quint64 chrono = iter.value().getChrono();
            if (!m_transactionsFinished.contains(chrono)) {
                qWarning("Unfinished SCPI chrono: %s (%llu)", qPrintable(iter.value().getScpi()), chrono);
                unfinished++;
            }
        }
        // TODO: Make critical
        qWarning("SCPI transactions pending: %i", unfinished);
    }
#endif
}

ScpiTransactionId ScpiResponseSorter::createTransaction(const QString &scpi)
{
    // * This is about sorting order of SCPI responses (not the order scpi commands/queries
    //   are executed)
    // * Since commands have no response from user perspective: just sort queries
    // * Archieve queries only by returning invalid transaction id for commands - see
    //   genOrDelaySortedOutput below
    bool bQuery = ScpiModuleCommonStaticFunctions::isQuery(scpi);
    if (!bQuery)
        return ScpiTransactionId();

    ScpiTransactionId id = ScpiTransactionId::createUniqueId(scpi);
    m_transactionsPending.insert(id.getChrono(), id);
    return id;
}

NullableStringList ScpiResponseSorter::genOrDelaySortedOutput(const NullableString &scpiSingleResponse, const ScpiTransactionId &scpiTransactionId)
{
    const quint64 chrono = scpiTransactionId.getChrono();
    ScpiTransactionId pendingTransaction = m_transactionsPending.take(chrono);
    if (pendingTransaction.isValid()) {
        m_transactionsFinished.insert(chrono, scpiSingleResponse);
        return createAccumulatedResponse();
    }
    NullableStringList ret;
    ret.append(scpiSingleResponse);
    return ret;
}

NullableStringList ScpiResponseSorter::createAccumulatedResponse()
{
    NullableStringList responseList;
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

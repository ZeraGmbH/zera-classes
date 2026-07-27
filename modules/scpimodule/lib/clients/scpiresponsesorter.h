#ifndef SCPIRESPONSESORTER_H
#define SCPIRESPONSESORTER_H

#include "scpiveintransactioninfo.h"
#include "nullablestring.h"
#include <QString>
#include <QMap>

class ScpiResponseSorter
{
public:
    virtual ~ScpiResponseSorter();
    ScpiTransactionId createTransaction(const QString &scpi);

    struct SortedResponse {
        NullableString scpiResponse;
        ScpiTransactionId scpiTransactionId;
        SCPIMODULE::FinishLogTypes logType;
    };
    typedef QList<SortedResponse> SortedResponseList;
    SortedResponseList genOrDelaySortedOutput(const NullableString &scpiSingleResponse,
                                              const ScpiTransactionId &scpiTransactionId,
                                              SCPIMODULE::FinishLogTypes logType = SCPIMODULE::LOG_FULL);
    SortedResponseList genImmediateNotSortedOutput(const NullableString &scpiSingleResponse,
                                                   const ScpiTransactionId &scpiTransactionId,
                                                   SCPIMODULE::FinishLogTypes logType);

private:
    SortedResponseList createAccumulatedResponse();

    QMap<quint64, ScpiTransactionId> m_transactionsPending;
    QMap<quint64, SortedResponse> m_transactionsFinished;
};

#endif // SCPIRESPONSESORTER_H

#ifndef SCPICMDRESPONSESORTER_H
#define SCPICMDRESPONSESORTER_H

#include "scpitransactionid.h"
#include <QString>
#include <QMap>

class ScpiCmdResponseSorter
{
public:
    ScpiTransactionId createTransaction();
    QStringList genOrDelaySortedOutput(const QString& scpiSingleResponse, const ScpiTransactionId &scpiTransactionId);

private:
    QStringList createAccumulatedResponse();

    QMap<quint64, ScpiTransactionId> m_transactionsPending;
    QMap<quint64, QString> m_transactionsFinished;
};

#endif // SCPICMDRESPONSESORTER_H

#ifndef SCPIRPCTRANSACTIONSTORE_H
#define SCPIRPCTRANSACTIONSTORE_H

#include "scpiveintransactioninfo.h"
#include <QMultiHash>

namespace SCPIMODULE {

class cSCPIClient;

class ScpiRpcTransactionStore
{
public:
    void addTransaction(const QString &rpcName, const SCPIVeinTransactionInfoPtr &transactionInfo);
    void removeClientTransactions(const cSCPIClient *client);
    void removeVeinTransaction(const QString &rpcName, const SCPIVeinTransactionInfoPtr &transactionInfo);

    const QList<SCPIVeinTransactionInfoPtr> getAllScpiVeinParamRpcTransactions();
    const QList<SCPIVeinTransactionInfoPtr> getScpiVeinParamRpcTransactions(const QString &rpcName);

private:
    QMultiHash<QString /*rpcName*/, SCPIVeinTransactionInfoPtr> m_scpiVeinParamOrRpcTransactions;
};

}
#endif // SCPIRPCTRANSACTIONSTORE_H

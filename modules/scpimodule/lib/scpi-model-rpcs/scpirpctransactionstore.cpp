#include "scpirpctransactionstore.h"

namespace SCPIMODULE {

void ScpiRpcTransactionStore::addTransaction(const QString &rpcName, const SCPIVeinTransactionInfoPtr &transactionInfo)
{
    m_scpiVeinParamOrRpcTransactions.insert(rpcName, transactionInfo);
}

void ScpiRpcTransactionStore::removeClientTransactions(const cSCPIClient *client)
{
    for(auto iter = m_scpiVeinParamOrRpcTransactions.begin(); iter != m_scpiVeinParamOrRpcTransactions.end(); ) {
        if(iter.value()->getClient() == client)
            iter = m_scpiVeinParamOrRpcTransactions.erase(iter);
        else
            iter++;
    }
}

void ScpiRpcTransactionStore::removeVeinTransaction(const QString &rpcName, const SCPIVeinTransactionInfoPtr &transactionInfo)
{
    m_scpiVeinParamOrRpcTransactions.remove(rpcName, transactionInfo);
}

const QList<SCPIVeinTransactionInfoPtr> ScpiRpcTransactionStore::getAllScpiVeinParamRpcTransactions()
{
    return m_scpiVeinParamOrRpcTransactions.values();
}

const QList<SCPIVeinTransactionInfoPtr> ScpiRpcTransactionStore::getScpiVeinParamRpcTransactions(const QString &rpcName)
{
    return m_scpiVeinParamOrRpcTransactions.values(rpcName);
}

}
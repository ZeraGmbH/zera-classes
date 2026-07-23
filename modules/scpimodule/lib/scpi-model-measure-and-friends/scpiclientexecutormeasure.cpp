#include "scpiclientexecutormeasure.h"
#include "scpiveincomponentsequencemeasure.h"
#include "scpiclient.h"
#include <zscpi_response_definitions.h>

namespace SCPIMODULE {

ScpiClientExecutorMeasure::ScpiClientExecutorMeasure(cSCPIClient *client,
                                                     const ScpiModelTypes modelType,
                                                     const QList<VeinComponentId> &veinComponents) :
    m_client(client),
    m_modelType(modelType)
{
    ScpiStoreScpiSequence* store = client->getMeasureSequenceStore();
    for (const VeinComponentId &componentId : veinComponents) {
        const VeinComponentScpiMeasureSequencePtr &measure = store->getComponentSequence(componentId);
        m_veinComponentScpiSequences.append(measure);

        switch (m_modelType) // don't get fooled: there is one connection made
        {
        case ScpiModelTypes::MEASURE:
            connect(measure.get(), &ScpiVeinComponentSequenceMeasure::sigMeasDone, this, &ScpiClientExecutorMeasure::onSingleScpiQueryDone);
            break;
        case ScpiModelTypes::CONFIGURE:
            connect(measure.get(), &ScpiVeinComponentSequenceMeasure::sigConfDone, this, &ScpiClientExecutorMeasure::onSingleScpiCmdDone);
            break;
        case ScpiModelTypes::READ:
            connect(measure.get(), &ScpiVeinComponentSequenceMeasure::sigReadDone, this, &ScpiClientExecutorMeasure::onSingleScpiQueryDone);
            break;
        case ScpiModelTypes::INIT:
            connect(measure.get(), &ScpiVeinComponentSequenceMeasure::sigInitDone, this, &ScpiClientExecutorMeasure::onSingleScpiCmdDone);
            break;
        case ScpiModelTypes::FETCH:
            connect(measure.get(), &ScpiVeinComponentSequenceMeasure::sigFetchDone, this, &ScpiClientExecutorMeasure::onSingleScpiQueryDone);
            break;
        }
    }
}

void ScpiClientExecutorMeasure::executeScpi(const ScpiTransactionId &scpiTransactionId)
{
    m_pendingTransactions[scpiTransactionId.getChrono()] = { scpiTransactionId, m_veinComponentScpiSequences.count(), "" };
    for (int i = 0; i < m_veinComponentScpiSequences.count(); i++) {
        const VeinComponentScpiMeasureSequencePtr &measure = m_veinComponentScpiSequences.at(i);
        measure->execute(m_modelType);
    }
}

void ScpiClientExecutorMeasure::onSingleScpiCmdDone()
{
    QList<quint64 /*TransactionId chrono */> toRemoveList;
    for (auto iter=m_pendingTransactions.begin(); iter != m_pendingTransactions.end(); iter++) {
        TransactionData &pendingTransaction = iter.value();
        int &pendingSequences = pendingTransaction.pendingSequences;
        pendingSequences--;
        if (pendingSequences == 0) {
            m_client->handleCmdFinishStatusOnly(ZSCPI::ack, pendingTransaction.transactionId);
            toRemoveList.append(iter.key());
        }
        else if (pendingSequences < 0)
            qCritical("cSCPIMeasureDelegate::onSingleScpiCmdDone: Pending < 0");
    }
    removeCompleteTransactions(toRemoveList);
}

void ScpiClientExecutorMeasure::onSingleScpiQueryDone(const QString &scpiResponse)
{
    QList<quint64 /*TransactionId chrono */> toRemoveList;
    for (auto iter=m_pendingTransactions.begin(); iter != m_pendingTransactions.end(); iter++) {
        TransactionData &pendingTransaction = iter.value();
        int &pendingSequences = pendingTransaction.pendingSequences;
        pendingSequences--;

        if ("FETCH:DFT1?" == pendingTransaction.transactionId.getScpi())
            qInfo("foo");

        QString &accumulatedResponse = pendingTransaction.accumulatedResponse;
        accumulatedResponse += QString("%1;").arg(scpiResponse);
        if (pendingSequences == 0) {
            toRemoveList.append(iter.key());
            m_client->handleCmdFinish(accumulatedResponse, pendingTransaction.transactionId);
        }
        else if (pendingSequences < 0)
            qCritical("cSCPIMeasureDelegate::onSingleScpiQueryDone: Pending < 0");
    }
    removeCompleteTransactions(toRemoveList);
}

void ScpiClientExecutorMeasure::removeCompleteTransactions(const QList<quint64> &toRemoveList)
{
    for (const quint64 &toRemove : toRemoveList)
        m_pendingTransactions.remove(toRemove);
}

}

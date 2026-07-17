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
    for (const VeinComponentId &componentId : veinComponents)
        m_veinComponentScpiSequences.append(store->getComponentSequence(componentId));
}

void ScpiClientExecutorMeasure::executeScpi(const ScpiTransactionId &scpiTransactionId)
{
    bool reentryPossible;
    switch (m_modelType)
    {
    case ScpiModelTypes::INIT:
    case ScpiModelTypes::CONFIGURE:
        reentryPossible = true;
        break;
    default:
        reentryPossible = false;
    }
    if (m_nPending == 0 || reentryPossible) { // not yet running or reentry
        m_nPending = m_veinComponentScpiSequences.count();
        m_sAnswer = "";
        for (int i = 0; i < m_veinComponentScpiSequences.count(); i++) {
            ScpiVeinComponentSequenceMeasure* measure = m_veinComponentScpiSequences.at(i).get();
            switch (m_modelType)
            {
            case ScpiModelTypes::MEASURE:
                connect(measure, &ScpiVeinComponentSequenceMeasure::sigMeasDone, this, &ScpiClientExecutorMeasure::onSingleScpiQueryDone);
                break;
            case ScpiModelTypes::CONFIGURE:
                connect(measure, &ScpiVeinComponentSequenceMeasure::sigConfDone, this, &ScpiClientExecutorMeasure::onSingleScpiCmdDone);
                break;
            case ScpiModelTypes::READ:
                connect(measure, &ScpiVeinComponentSequenceMeasure::sigReadDone, this, &ScpiClientExecutorMeasure::onSingleScpiQueryDone);
                break;
            case ScpiModelTypes::INIT:
                connect(measure, &ScpiVeinComponentSequenceMeasure::sigInitDone, this, &ScpiClientExecutorMeasure::onSingleScpiCmdDone);
                break;
            case ScpiModelTypes::FETCH:
                connect(measure, &ScpiVeinComponentSequenceMeasure::sigFetchDone, this, &ScpiClientExecutorMeasure::onSingleScpiQueryDone);
                break;
            }
            measure->execute(m_modelType, scpiTransactionId);
        }
    }
    else
        m_client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void ScpiClientExecutorMeasure::onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const ScpiVeinComponentSequenceMeasure *sender)
{
    disconnect(sender, 0, this, 0);

    m_nPending--;
    if (m_nPending == 0)
        m_client->handleCmdFinishStatusOnly(ZSCPI::ack, scpiTransactionId);
    else if (m_nPending < 0)
        qCritical("cSCPIMeasureDelegate::onSingleScpiCmdDone: m_nPending < 0");
}

void ScpiClientExecutorMeasure::onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const ScpiVeinComponentSequenceMeasure *sender)
{
    disconnect(sender, 0, this, 0);

    m_sAnswer += QString("%1;").arg(scpiResponse);

    m_nPending--;
    if (m_nPending == 0)
        m_client->handleCmdFinish(m_sAnswer, scpiTransactionId);
    else if (m_nPending < 0)
        qCritical("cSCPIMeasureDelegate::onSingleScpiQueryDone: m_nPending < 0");
}

}

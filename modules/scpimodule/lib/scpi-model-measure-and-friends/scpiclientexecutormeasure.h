#ifndef SCPICLIENTEXECUTORMEASURE_H
#define SCPICLIENTEXECUTORMEASURE_H

#include "scpidelegatetemplate.h"
#include "scpiveincomponentsequencemeasure.h"
#include <QList>

namespace SCPIMODULE {

class ScpiClientExecutorMeasure : public QObject
{
    Q_OBJECT
public:
    explicit ScpiClientExecutorMeasure(cSCPIClient *client,
                                       const ScpiModelTypes modelType,
                                       const QList<VeinComponentId> &veinComponents);

    void executeScpi(const ScpiTransactionId &scpiTransactionId);

private slots:
    void onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::ScpiVeinComponentSequenceMeasure* sender);
    void onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::ScpiVeinComponentSequenceMeasure* sender);

private:
    void executeClient(const ScpiTransactionId &scpiTransactionId);

    cSCPIClient *m_client = nullptr;
    const ScpiModelTypes m_modelType;

    QList<VeinComponentScpiMeasureSequencePtr> m_veinComponentScpiSequences;
    int m_nPending = 0;
    QString m_sAnswer;
};

typedef std::shared_ptr<ScpiClientExecutorMeasure> ScpiClientExecutorMeasurePtr;

}

#endif // SCPICLIENTEXECUTORMEASURE_H

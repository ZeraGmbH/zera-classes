#ifndef SCPICLIENTEXECUTORMEASURE_H
#define SCPICLIENTEXECUTORMEASURE_H

#include "scpidelegatetemplate.h"
#include "scpiveincomponentsequencemeasure.h"
#include <QList>
#include <QHash>

namespace SCPIMODULE {

class ScpiClientExecutorMeasure : public QObject
{
    Q_OBJECT
public:
    explicit ScpiClientExecutorMeasure(cSCPIClient *client,
                                       const ScpiModelTypes modelType,
                                       const QList<VeinComponentId> &veinComponents);
    ~ScpiClientExecutorMeasure() override;

    void executeScpi(const ScpiTransactionId &scpiTransactionId);

    static int getInstanceCount();
private slots:
    void onSingleScpiCmdDone();
    void onSingleScpiQueryDone(const QString &scpiResponse);

private:
    void removeCompleteTransactions(const QList<quint64 /*TransactionId chrono */> &toRemoveList);

    cSCPIClient *m_client = nullptr;
    const ScpiModelTypes m_modelType;

    QList<VeinComponentScpiMeasureSequencePtr> m_veinComponentScpiSequences;
    struct TransactionData {
        ScpiTransactionId transactionId;
        int pendingSequences = 0;
        QString accumulatedResponse;
    };
    QHash<quint64 /*TransactionId chrono */, TransactionData> m_pendingTransactions;
    static int m_instanceCount;
};

typedef std::shared_ptr<ScpiClientExecutorMeasure> ScpiClientExecutorMeasurePtr;

}

#endif // SCPICLIENTEXECUTORMEASURE_H

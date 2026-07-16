#ifndef SCPICLIENTMEASUREEXECUTOR_H
#define SCPICLIENTMEASUREEXECUTOR_H

#include "scpidelegatetemplate.h"
#include "veincomponentscpimeasuresequence.h"
#include <QList>

namespace SCPIMODULE {

class ScpiClientMeasureExecutor : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    struct Params {
        const QString cmdParent;
        const QString cmd;
        const quint8 scpiQueryCmdFlags = 0;
        const ScpiModelTypes modelType = measure;
        VeinComponentScpiMeasureSequence* scpiMeasureObject = nullptr;
    };
    // Moduleinterface initials
    explicit ScpiClientMeasureExecutor(const Params &params);
    // Scpi interface copies
    ScpiClientMeasureExecutor(const ScpiClientMeasureExecutor& moduleInterfaceDelegate,
                        QHash<VeinComponentScpiMeasureSequence*, VeinComponentScpiMeasureSequence*> &scpiMeasureTranslationHash);

    void addVeinComponentScpiSequence(VeinComponentScpiMeasureSequence* measureobject);

    void executeSCPI(cSCPIClient *client, const QString& scpi, const ScpiTransactionId &scpiTransactionId) override;
    void executeClient(cSCPIClient *client, const ScpiTransactionId &scpiTransactionId);

private slots:
    void onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);
    void onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);

private:
    const ScpiModelTypes m_modelType;
    int m_nPending = 0;
    QString m_sAnswer;
    cSCPIClient *m_client = nullptr;
    QList<VeinComponentScpiMeasureSequence*> m_veinComponentScpiSequences;
};

typedef std::shared_ptr<ScpiClientMeasureExecutor> ScpiDelegateMeasurePtr;

}

#endif // SCPICLIENTMEASUREEXECUTOR_H

#ifndef SCPIDELEGATEMEASURE_H
#define SCPIDELEGATEMEASURE_H

#include "scpidelegatetemplate.h"
#include "veincomponentscpimeasuresequence.h"
#include <QList>

namespace SCPIMODULE {

class ScpiDelegateMeasure : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    // Moduleinterface initials
    ScpiDelegateMeasure(const QString &cmdParent,
                         const QString &cmd,
                         quint8 scpiCmdQueryFlags,
                         ScpiModelTypes modelType,
                         VeinComponentScpiMeasureSequence* scpimeasureobject);
    // Scpi interface copies
    ScpiDelegateMeasure(const ScpiDelegateMeasure& moduleInterfaceDelegate,
                         QHash<VeinComponentScpiMeasureSequence*, VeinComponentScpiMeasureSequence*> &scpiMeasureTranslationHash);

    void addVeinComponentScpiSequence(VeinComponentScpiMeasureSequence* measureobject);

    void executeSCPI(cSCPIClient *client, const QString& scpi, const ScpiTransactionId &scpiTransactionId) override;
    void executeClient(cSCPIClient *client, const ScpiTransactionId &scpiTransactionId);

private slots:
    void onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);
    void onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);

private:
    ScpiModelTypes m_modelType;
    int m_nPending = 0;
    QString m_sAnswer;
    cSCPIClient *m_pClient = nullptr;
    QList<VeinComponentScpiMeasureSequence*> m_veinComponentScpiSequences;
};

typedef std::shared_ptr<ScpiDelegateMeasure> MeasureScpiNodeDelegatePtr;

}

#endif // SCPIDELEGATEMEASURE_H

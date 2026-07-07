#ifndef SCPIMEASUREDELEGATE_H
#define SCPIMEASUREDELEGATE_H

#include "scpibasedelegate.h"
#include "veincomponentscpimeasuresequence.h"
#include <QList>

namespace SCPIMODULE {

class MeasureScpiNodeDelegate: public ScpiBaseDelegate
{
    Q_OBJECT
public:
    // Moduleinterface initials
    MeasureScpiNodeDelegate(const QString &cmdParent,
                         const QString &cmd,
                         quint8 scpiCmdQueryFlags,
                         ScpiModelTypes modelType,
                         VeinComponentScpiMeasureSequence* scpimeasureobject);
    // Scpi interface copies
    MeasureScpiNodeDelegate(const MeasureScpiNodeDelegate& moduleInterfaceDelegate,
                         QHash<VeinComponentScpiMeasureSequence*, VeinComponentScpiMeasureSequence*> &scpiMeasureTranslationHash);
    void executeSCPI(cSCPIClient *client, const QString& scpi, const ScpiTransactionId &scpiTransactionId) override;
    void executeClient(cSCPIClient *client, const ScpiTransactionId &scpiTransactionId);
    void addScpiMeasureObject(VeinComponentScpiMeasureSequence* measureobject);

private slots:
    void onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);
    void onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);

private:
    ScpiModelTypes m_modelType;
    int m_nPending = 0;
    QString m_sAnswer;
    cSCPIClient *m_pClient = nullptr;
    QList<VeinComponentScpiMeasureSequence*> m_veinComponentSequences;
};

typedef std::shared_ptr<MeasureScpiNodeDelegate> MeasureScpiNodeDelegatePtr;

}

#endif // SCPIMEASUREDELEGATE_H

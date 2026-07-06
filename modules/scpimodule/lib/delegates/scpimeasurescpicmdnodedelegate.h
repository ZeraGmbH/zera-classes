#ifndef SCPIMEASUREDELEGATE_H
#define SCPIMEASUREDELEGATE_H

#include "scpibasedelegate.h"
#include "scpimeasureperveincomponentsequencer.h"
#include <QList>

namespace SCPIMODULE {

class ScpiMeasureScpiCmdNodeDelegate: public ScpiBaseDelegate
{
    Q_OBJECT
public:
    // Moduleinterface initials
    ScpiMeasureScpiCmdNodeDelegate(const QString &cmdParent,
                         const QString &cmd,
                         quint8 scpiCmdQueryFlags,
                         ScpiModelTypes modelType,
                         ScpiMeasurePerVeinComponentSequencer* scpimeasureobject);
    // Scpi interface copies
    ScpiMeasureScpiCmdNodeDelegate(const ScpiMeasureScpiCmdNodeDelegate& moduleInterfaceDelegate,
                         QHash<ScpiMeasurePerVeinComponentSequencer*, ScpiMeasurePerVeinComponentSequencer*> &scpiMeasureTranslationHash);
    void executeSCPI(cSCPIClient *client, const QString& scpi, const ScpiTransactionId &scpiTransactionId) override;
    void executeClient(cSCPIClient *client, const ScpiTransactionId &scpiTransactionId);
    void addScpiMeasureObject(ScpiMeasurePerVeinComponentSequencer* measureobject);

private slots:
    void onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::ScpiMeasurePerVeinComponentSequencer* sender);
    void onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::ScpiMeasurePerVeinComponentSequencer* sender);

private:
    ScpiModelTypes m_modelType;
    int m_nPending = 0;
    QString m_sAnswer;
    cSCPIClient *m_pClient = nullptr;
    QList<ScpiMeasurePerVeinComponentSequencer*> m_scpimeasureObjectList;
};

typedef std::shared_ptr<ScpiMeasureScpiCmdNodeDelegate> ScpiMeasureScpiCmdNodeDelegatePtr;

}

#endif // SCPIMEASUREDELEGATE_H

#ifndef SCPIMEASUREDELEGATE_H
#define SCPIMEASUREDELEGATE_H

#include "scpibasedelegate.h"
#include "perveincomponentmeasuretransaction.h"
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
                         PerVeinComponentMeasureTransaction* scpimeasureobject);
    // Scpi interface copies
    ScpiMeasureScpiCmdNodeDelegate(const ScpiMeasureScpiCmdNodeDelegate& moduleInterfaceDelegate,
                         QHash<PerVeinComponentMeasureTransaction*, PerVeinComponentMeasureTransaction*> &scpiMeasureTranslationHash);
    void executeSCPI(cSCPIClient *client, const QString& scpi, const ScpiTransactionId &scpiTransactionId) override;
    void executeClient(cSCPIClient *client, const ScpiTransactionId &scpiTransactionId);
    void addScpiMeasureObject(PerVeinComponentMeasureTransaction* measureobject);

private slots:
    void onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::PerVeinComponentMeasureTransaction* sender);
    void onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::PerVeinComponentMeasureTransaction* sender);

private:
    ScpiModelTypes m_modelType;
    int m_nPending = 0;
    QString m_sAnswer;
    cSCPIClient *m_pClient = nullptr;
    QList<PerVeinComponentMeasureTransaction*> m_scpimeasureObjectList;
};

typedef std::shared_ptr<ScpiMeasureScpiCmdNodeDelegate> ScpiMeasureScpiCmdNodeDelegatePtr;

}

#endif // SCPIMEASUREDELEGATE_H

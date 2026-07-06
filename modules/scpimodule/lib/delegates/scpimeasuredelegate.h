#ifndef SCPIMEASUREDELEGATE_H
#define SCPIMEASUREDELEGATE_H

#include "scpibasedelegate.h"
#include "scpimeasure.h"
#include <QList>

namespace SCPIMODULE {

class cSCPIMeasureDelegate: public ScpiBaseDelegate
{
    Q_OBJECT
public:
    // Moduleinterface initials
    cSCPIMeasureDelegate(const QString &cmdParent,
                         const QString &cmd,
                         quint8 scpiCmdQueryFlags,
                         ScpiModelTypes modelType,
                         cSCPIMeasure* scpimeasureobject);
    // Scpi interface copies
    cSCPIMeasureDelegate(const cSCPIMeasureDelegate& moduleInterfaceDelegate,
                         QHash<cSCPIMeasure*, cSCPIMeasure*> &scpiMeasureTranslationHash);
    void executeSCPI(cSCPIClient *client, const QString& scpi, const ScpiTransactionId &scpiTransactionId) override;
    void executeClient(cSCPIClient *client, const ScpiTransactionId &scpiTransactionId);
    void addScpiMeasureObject(cSCPIMeasure* measureobject);

private slots:
    void onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::cSCPIMeasure* sender);
    void onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::cSCPIMeasure* sender);

private:
    ScpiModelTypes m_modelType;
    int m_nPending = 0;
    QString m_sAnswer;
    cSCPIClient *m_pClient = nullptr;
    QList<cSCPIMeasure*> m_scpimeasureObjectList;
};

typedef std::shared_ptr<cSCPIMeasureDelegate> cSCPIMeasureDelegatePtr;

}

#endif // SCPIMEASUREDELEGATE_H

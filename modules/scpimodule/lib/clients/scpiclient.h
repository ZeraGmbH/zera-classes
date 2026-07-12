#ifndef SCPICLIENT_H
#define SCPICLIENT_H

#include "scpiinterface.h"
#include "measurescpinodedelegate.h"
#include "signalconnectiondelegate.h"
#include "scpiveintransactioninfo.h"
#include "ieee488-2.h"
#include "scpimoduleconfigdata.h"
#include "scpiresponsesorter.h"
#include <QUuid>
#include <QHash>

namespace SCPIMODULE
{

class cSCPIModule;

class cSCPIClient: public QObject
{
    Q_OBJECT
public:
    cSCPIClient(cSCPIModule* module);
    virtual ~cSCPIClient();

    cSCPIInterface* getScpiInterface();
    cSCPIStatus* getSCPIStatus(SCPIStatusDefinitions::ScpiStatusSystems statusSystemIdx);
    cIEEE4882* getIEEE4882();

    bool isOperationComplete();

    QUuid getClientId();
    void addVeinParamRpcTransactionInfo(const QString &veinComponentOrRpcName, const SCPIMODULE::SCPIVeinTransactionInfoPtr &info);

    QHash<MeasureScpiNodeDelegate*, MeasureScpiNodeDelegatePtr> m_SCPIMeasureDelegateHash;

    enum FinishLogTypes {
        LOG_FULL,
        LOG_SKIP
    };
    virtual void handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) = 0;
    void handleCmdFinishStatusOnly(quint8 stat, const ScpiTransactionId &scpiTransactionId);
public slots:
    void removeVeinParamRpcTransactionInfo(const QString &veinComponentOrRpcName);

protected:
    cSCPIModule* m_pModule;
    cSCPIInterface* m_pSCPIInterface;
    ScpiResponseSorter m_responseSorter;

    QString m_sInputFifo;
    QChar m_endChar;

    int execPendingCmds();
    bool cmdAvail();
    void takeCmd();
    void execCmd();
    QString makeBareScpiInPrintable(const QString &input);

private slots:
    virtual void cmdInput() = 0;
private:
    typedef QHash<QString /*entityName*/, int /*entityId*/> EntityHash;
    void setSignalConnections(cSCPIStatus* scpiStatus,
                              const QList<cStatusBitDescriptor> &statusBitDescriptorList,
                              const EntityHash &entitiesWithScpi);
    EntityHash getEntitiesWithScpi() const;
    void generateSCPIMeasureSystem();

    cSCPIModuleConfigData& m_ConfigData;
    // what happens on same component names in different entities as e.g THDN module??? => TODO test
    QHash<QString /* veinComponentOrRpcName */, SCPIVeinTransactionInfoPtr> m_veinParamRpcTransactionHash;
    cIEEE4882* m_pIEEE4882 = nullptr;

    QList<cSCPIStatus*> m_SCPIStatusList;

    QHash<VeinComponentScpiMeasureSequence*, VeinComponentScpiMeasureSequence*> m_SCPIMeasureTranslationHash;
    QUuid m_clientId;

    QList<cSignalConnectionDelegate*> m_connectDelegateList;

    QString m_activeCmd;
};

}
#endif // SCPICLIENT_H

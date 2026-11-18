#ifndef SCPICLIENT_H
#define SCPICLIENT_H

#include "scpiinterface.h"
#include "scpimeassequencestore.h"
#include "signalconnectiondelegate.h"
#include "scpiveintransactioninfo.h"
#include "ieee488-2.h"
#include "scpimoduleconfigdata.h"
#include "scpiresponsesorter.h"
#include "scpiveinparser.h"
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
    ScpiMeasSequenceStore* getMeasureSequenceStore();

    bool isOperationComplete();

    const QUuid &getClientId() const;
    void addVeinParamRpcTransactionInfo(const QString &veinComponentOrRpcName, const SCPIMODULE::SCPIVeinTransactionInfoPtr &info);

    virtual void handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) = 0;
    void handleCmdFinishStatusOnly(quint8 stat, const ScpiTransactionId &scpiTransactionId);

    static int getInstanceCount();
signals:
    void sigAboutToDestroy();

public slots:
    void removeVeinParamRpcTransactionInfo(const QString &veinComponentOrRpcName);

protected:
    int execPendingCmds();
    bool cmdAvail();
    void takeCmd();
    void execCmd();
    QString makeBareScpiInPrintable(const QString &input);

    cSCPIModule* m_pModule = nullptr;
    cSCPIInterface* m_pSCPIInterface = nullptr;
    ScpiResponseSorter m_responseSorter;

    QString m_sInputFifo;
    QChar m_endChar;

private slots:
    virtual void cmdInput() = 0;
private:
    void setSignalConnections(cSCPIStatus* scpiStatus,
                              const QList<cStatusBitDescriptor> &statusBitDescriptorList,
                              const ScpiVeinParser::ScpiEntityHash &entitiesWithScpi);

    cSCPIModuleConfigData& m_ConfigData;
    // what happens on same component names in different entities as e.g THDN module??? => TODO test
    QHash<QString /* veinComponentOrRpcName */, SCPIVeinTransactionInfoPtr> m_veinParamRpcTransactionHash;
    cIEEE4882* m_pIEEE4882 = nullptr;
    ScpiMeasSequenceStore m_measureSequenceStore;

    QList<cSCPIStatus*> m_SCPIStatusList;

    QUuid m_clientId;

    QList<cSignalConnectionDelegate*> m_connectDelegateList;

    QString m_activeCmd;

    static int m_instanceCount;
};

}
#endif // SCPICLIENT_H

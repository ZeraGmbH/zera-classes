#ifndef SCPICLIENT_H
#define SCPICLIENT_H

#include "scpiinterface.h"
#include "scpistatus.h"
#include "scpimeasuredelegate.h"
#include "scpiclientinfo.h"
#include "ieee488-2.h"
#include "scpimoduleconfigdata.h"
#include "signalconnectiondelegate.h"
#include <QUuid>
#include <QHash>

namespace SCPIMODULE
{

class cSCPIModule;

class cSCPIClient: public QObject
{
    Q_OBJECT
public:
    cSCPIClient(cSCPIModule* module, cSCPIModuleConfigData& configdata, cSCPIInterface* iface);
    virtual ~cSCPIClient();

    void setAuthorisation(bool auth);
    cSCPIStatus* getSCPIStatus(quint8 index);
    quint8 operationComplete();
    cIEEE4882* getIEEE4882();
    QUuid getClientId();
    void addSCPIClientInfo(QString key, SCPIMODULE::SCPIClientInfoPtr info);

    QHash<cSCPIMeasureDelegate*, cSCPIMeasureDelegatePtr> m_SCPIMeasureDelegateHash;
signals:
    void commandAnswered(SCPIMODULE::cSCPIClient* client);
public slots:
    void receiveStatus(quint8 stat);
    virtual void receiveAnswer(QString answ, bool ok = true, bool skipLog = false) = 0;
    void removeSCPIClientInfo(const QString &key);

protected:
    cSCPIInterface* m_pSCPIInterface;

    QString m_sInputFifo;
    QChar m_endChar;

    void execPendingCmds();
    bool cmdAvail();
    void takeCmd();
    void execCmd();
    QString makeBareScpiInPrintable(const QString &input);

private slots:
    virtual void cmdInput() = 0;
private:
    void setSignalConnections(cSCPIStatus* scpiStatus, const QList<cStatusBitDescriptor> &statusBitDescriptorList);
    void generateSCPIMeasureSystem();

    cSCPIModule* m_pModule;
    cSCPIModuleConfigData& m_ConfigData;
    QHash<QString, SCPIClientInfoPtr> m_scpiClientInfoHash;
    cIEEE4882* m_pIEEE4882 = nullptr;

    QList<cSCPIStatus*> m_SCPIStatusList;

    QHash<cSCPIMeasure*, cSCPIMeasure*> m_SCPIMeasureTranslationHash;
    QUuid m_clientId;

    bool m_bAuthorisation = false;
    QList<cSignalConnectionDelegate*> m_connectDelegateList;

    QString m_activeCmd;
};

}
#endif // SCPICLIENT_H

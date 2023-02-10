#ifndef SCPICLIENT_H
#define SCPICLIENT_H

#include <QUuid>
#include <QHash>
#include "scpiinterface.h"
#include "scpistatus.h"
#include "scpimeasuredelegate.h"
#include "scpiclientinfo.h"
#include "ieee488-2.h"
#include "scpimoduleconfigdata.h"
#include "signalconnectiondelegate.h"

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

    QHash<cSCPIMeasureDelegate*, cSCPIMeasureDelegate*> m_SCPIMeasureDelegateHash;

public slots:
    void receiveStatus(quint8 stat);
    virtual void receiveAnswer(QString answ) = 0;
    void addSCPIClientInfo(QString key, cSCPIClientInfo* info);
    void removeSCPIClientInfo(QString key);

protected:
    cSCPIInterface* m_pSCPIInterface;
    cIEEE4882* m_pIEEE4882;
    QHash<QString, cSCPIClientInfo*> m_scpiClientInfoHash;

    QString m_sInputFifo;
    QString activeCmd;
    QChar endChar;

    void testCmd();
    bool cmdAvail();
    void takeCmd();
    void execCmd();

private:
    cSCPIModule* m_pModule;
    cSCPIModuleConfigData& m_ConfigData;

    QList<cSCPIStatus*> m_SCPIStatusList;

    QHash<cSCPIMeasure*, cSCPIMeasure*> m_SCPIMeasureTranslationHash;
    QUuid mClientId;

    bool m_bAuthorisation;
    void setSignalConnections(cSCPIStatus* scpiStatus, QList<cStatusBitDescriptor> &dList);
    QList<cSignalConnectionDelegate*> mysConnectDelegateList;
    void generateSCPIMeasureSystem();

private slots:
    virtual void cmdInput() = 0;

};

}
#endif // SCPICLIENT_H

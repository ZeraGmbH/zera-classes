#ifndef SCPIMODULE_H
#define SCPIMODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QHash>
#include <QMultiHash>

#include "scpiclientinfo.h"
#include "basemodule.h"


namespace Zera {
namespace Server {
 class cDSPInterface;
}

namespace Proxy {
 class cProxy;
}
}

namespace VeinEvent
{
    class EventSystem;
    class StorageSystem;
}

namespace SCPIMODULE
{

#define BaseModuleName "SCPIModule"
#define BaseSCPIModuleName "SCP"

class cSCPIServer;
class cSignalConnectionDelegate;
class cSCPIMeasure;
class cSCPIEventSystem;


class cSCPIModule : public cBaseModule
{
    Q_OBJECT

public:
    cSCPIModule(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem *storagesystem, QObject* parent = 0);
    virtual ~cSCPIModule();
    virtual QByteArray getConfiguration() const;

    QHash<QString, cSCPIClientInfo*> scpiClientInfoHash;
    QHash<QString, cSignalConnectionDelegate*> sConnectDelegateHash;
    QMultiHash<QString, cSCPIMeasure*> scpiMeasureHash;

    cSCPIEventSystem* m_pSCPIEventSystem;

    QHash<QString, cSCPIClientInfo*> scpiClientInfoHash;
    QHash<QString, cSignalConnectionDelegate*> sConnectDelegateHash;
    QMultiHash<QString, cSCPIMeasure*> scpiMeasureHash;

    cSCPIEventSystem* m_pSCPIEventSystem;

protected:
    cSCPIServer *m_pSCPIServer; // our server for the world

    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void unsetModule(); // in case of reconfiguration we must unset module first
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();

protected slots:
    virtual void activationStart();
    virtual void activationExec();
    virtual void activationDone();
    virtual void activationFinished();

    virtual void deactivationStart();
    virtual void deactivationExec();
    virtual void deactivationDone();
    virtual void deactivationFinished();

private:
    qint32 m_nActivationIt;


private slots:
    void scpiModuleReconfigure();

};

}

#endif // SCPIMODULE_H

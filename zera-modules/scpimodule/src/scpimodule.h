#ifndef SCPIMODULE_H
#define SCPIMODULE_H

#include "scpiclientinfo.h"
#include <basemodule.h>
#include "vfmoduleparameventsytem.h"
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QHash>
#include <QMultiHash>


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
    virtual QByteArray getConfiguration() const;
    cSCPIServer* getSCPIServer();

    QMultiHash<QString, cSCPIClientInfo*> scpiParameterCmdInfoHash; // a hash to memorize what was in progress for which client
    QList<cSignalConnectionDelegate*> sConnectDelegateList;
    QMultiHash<QString, cSCPIMeasure*> scpiMeasureHash;

    cSCPIEventSystem* m_pSCPIEventSystem;
    VfModuleParamEventSytem* m_pModuleValidator;

protected:
    cSCPIServer *m_pSCPIServer; // our server for the world

    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
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
};

}

#endif // SCPIMODULE_H

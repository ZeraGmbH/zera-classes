#ifndef SCPIMODULE_H
#define SCPIMODULE_H

#include "scpiclientinfo.h"
#include "basemodule.h"
#include "vfeventsytemmoduleparam.h"
#include <QFinalState>

namespace VeinEvent
{
    class EventSystem;
    class StorageSystem;
}

namespace SCPIMODULE {

class cSCPIServer;
class cSignalConnectionDelegate;
class cSCPIMeasure;
class cSCPIEventSystem;


class cSCPIModule : public cBaseModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SCPIModule";
    static constexpr const char* BaseSCPIModuleName = "SCP";

    cSCPIModule(MeasurementModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;
    cSCPIServer* getSCPIServer();

    QMultiHash<QString, SCPIClientInfoPtr> scpiParameterCmdInfoHash; // a hash to memorize what was in progress for which client
    QList<cSignalConnectionDelegate*> sConnectDelegateList;
    QMultiHash<QString, cSCPIMeasure*> scpiMeasureHash;

    cSCPIEventSystem* m_pSCPIEventSystem;
    VfEventSytemModuleParam* m_pModuleValidator;

protected:
    cSCPIServer *m_pSCPIServer; // our server for the world

    virtual void setupModule() override; // after xml configuration we can setup and export our module
    virtual void startMeas() override; // we make the measuring program start here
    virtual void stopMeas() override;

protected slots:
    virtual void activationStart() override;
    virtual void activationExec() override;
    virtual void activationDone() override;
    virtual void activationFinished() override;

    virtual void deactivationStart() override;
    virtual void deactivationExec() override;
    virtual void deactivationDone() override;
    virtual void deactivationFinished() override;

private:
    qint32 m_nActivationIt;
};

}

#endif // SCPIMODULE_H

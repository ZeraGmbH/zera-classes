#ifndef SOURCEMODULE_H
#define SOURCEMODULE_H

#include "sourcemoduleprogram.h"
#include <basemeasmodule.h>
#include <vfmodulerpc.h>

class SourceModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SourceModule";
    static constexpr const char* BaseSCPIModuleName = "SRC";

    SourceModule(quint8 modnr, int entityId, VeinEvent::StorageSystem *storagesystem, bool demo);
    virtual QByteArray getConfiguration() const;
    VfModuleRpc *getRpcEventSystem() const;

protected:
    SourceModuleProgram *m_pProgram; // our program, lets say the working horse

    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();

private slots:
    // entered immediately - no interim states for now
    void activationFinished();
    void deactivationFinished();

    // sigh - we have to add overrides to not being pure virtual
    virtual void activationStart() {}
    virtual void activationExec() {}
    virtual void activationDone() {}

    virtual void deactivationStart() {}
    virtual void deactivationExec() {}
    virtual void deactivationDone() {}
private:
    QState m_stateSwitchAllOff;
    VfModuleRpc *m_rpcEventSystem;
};

#endif // SOURCEMODULE_H

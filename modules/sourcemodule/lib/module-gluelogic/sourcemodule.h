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

    SourceModule(MeasurementModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;
    VfModuleRpc *getRpcEventSystem() const;

protected:
    SourceModuleProgram *m_pProgram; // our program, lets say the working horse

    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

private slots:
    // entered immediately - no interim states for now
    void activationFinished() override;
    void deactivationFinished() override;

    // sigh - we have to add overrides to not being pure virtual
    void activationStart() override {}
    void activationExec() override {}
    void activationDone() override {}

    void deactivationStart() override {}
    void deactivationExec() override {}
    void deactivationDone() override {}
private:
    QState m_stateSwitchAllOff;
    VfModuleRpc *m_rpcEventSystem;
};

#endif // SOURCEMODULE_H

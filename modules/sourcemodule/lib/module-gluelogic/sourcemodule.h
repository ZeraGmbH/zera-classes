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

    explicit SourceModule(ModuleFactoryParam moduleParam);
    VfModuleRpc *getRpcEventSystem() const;

protected:
    SourceModuleProgram *m_pProgram = nullptr;

    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

private:
    QState m_stateSwitchAllOff;
    VfModuleRpc *m_rpcEventSystem = nullptr;
};

#endif // SOURCEMODULE_H

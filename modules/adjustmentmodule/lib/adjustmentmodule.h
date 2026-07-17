#ifndef ADJUSTMENTMODULE_H
#define ADJUSTMENTMODULE_H

#include "adjustmentmoduleconfiguration.h"
#include "adjustmentmodulemeasprogram.h"
#include <vfrpceventsystemsimplified.h>
#include <basemeasmodule.h>

class cAdjustmentModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "AdjustmentModule";
    static constexpr const char* BaseSCPIModuleName = "ADJ";

    explicit cAdjustmentModule(const ModuleFactoryParam &moduleParam);

    cAdjustmentModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

    VfRpcEventSystemSimplified *getRpcEventSystem();

private slots:
    void activationFinished() override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cAdjustmentModuleMeasProgram *m_pMeasProgram = nullptr;
    cAdjustmentModuleConfiguration m_configuration;
    std::unique_ptr<VfRpcEventSystemSimplified> m_spRpcEventSystem;
};

#endif // ADJUSTMENTMODULE_H

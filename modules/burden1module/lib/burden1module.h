#ifndef BURDEN1MODULE_H
#define BURDEN1MODULE_H

#include "burden1moduleconfiguration.h"
#include "burden1modulemeasprogram.h"
#include <basemeasmodule.h>

namespace BURDEN1MODULE
{
class cBurden1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "Burden1Module";
    static constexpr const char* BaseSCPIModuleName = "BD1";

    explicit cBurden1Module(const ModuleFactoryParam &moduleParam);
    cBurden1ModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cBurden1ModuleMeasProgram *m_pMeasProgram = nullptr;
    cBurden1ModuleConfiguration m_configuration;
};

}

#endif // BURDEN1MODULE_H
